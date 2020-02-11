#include <ps4app/session.h>
#include <ps4app/http.h>
#include <ps4app/base64.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <Ws2tcpip.h>
#include <winsock.h>


#define SESSION_PORT					9295

#define RP_APPLICATION_REASON_IN_USE	0x80108b10
#define RP_APPLICATION_REASON_CRASH		0x80108b15


static void *session_thread_func(void *arg);


PS4APP_EXPORT Ps4AppErrorCode ps4app_session_init(Ps4AppSession *session, Ps4AppConnectInfo *connect_info)
{
	memset(session, 0, sizeof(Ps4AppSession));

	session->quit_reason = PS4APP_QUIT_REASON_NONE;

	int r = getaddrinfo(connect_info->host, NULL, NULL, &session->connect_info.host_addrinfos);
	if(r != 0)
	{
		ps4app_session_finish(session);
		return PS4APP_ERR_PARSE_ADDR;
	}

	session->connect_info.regist_key = strdup(connect_info->regist_key);
	if(!session->connect_info.regist_key)
	{
		ps4app_session_finish(session);
		return PS4APP_ERR_MEMORY;
	}

	session->connect_info.ostype = strdup(connect_info->ostype);
	if(!session->connect_info.regist_key)
	{
		ps4app_session_finish(session);
		return PS4APP_ERR_MEMORY;
	}

	memcpy(session->connect_info.auth, connect_info->auth, sizeof(session->connect_info.auth));
	memcpy(session->connect_info.morning, connect_info->morning, sizeof(session->connect_info.morning));

	return PS4APP_ERR_SUCCESS;
}

PS4APP_EXPORT void ps4app_session_finish(Ps4AppSession *session)
{
	if(!session)
		return;
	free(session->connect_info.regist_key);
	free(session->connect_info.ostype);
	freeaddrinfo(session->connect_info.host_addrinfos);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_session_start(Ps4AppSession *session)
{
	return ps4app_thread_create(&session->session_thread, session_thread_func, session);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_session_join(Ps4AppSession *session)
{
	return ps4app_thread_join(&session->session_thread, NULL);
}

static void session_send_event(Ps4AppSession *session, Ps4AppEvent *event)
{
	if(!session->event_cb)
		return;
	session->event_cb(event, session->event_cb_user);
}


static bool session_thread_request_session(Ps4AppSession *session);

static void *session_thread_func(void *arg)
{
	Ps4AppSession *session = arg;
	bool success;

	success = session_thread_request_session(session);
	if(!success)
		goto quit;

	printf("Connected!\n");

	Ps4AppEvent quit_event;
quit:
	quit_event.type = PS4APP_EVENT_QUIT;
	quit_event.quit.reason = session->quit_reason;
	session_send_event(session, &quit_event);
	return NULL;
}




typedef struct session_response_t {
	uint32_t error_code;
	const char *nonce;
	const char *rp_version;
	bool success;
} SessionResponse;

static void parse_session_response(SessionResponse *response, Ps4AppHttpResponse *http_response)
{
	memset(response, 0, sizeof(SessionResponse));

	if(http_response->code == 200)
	{
		for(Ps4AppHttpHeader *header=http_response->headers; header; header=header->next)
		{
			if(strcmp(header->key, "RP-Nonce") == 0)
				response->nonce = header->value;
			else if(strcmp(header->key, "RP-Version") == 0)
				response->rp_version = header->value;
		}
		response->success = response->nonce != NULL;
	}
	else
	{
		for(Ps4AppHttpHeader *header=http_response->headers; header; header=header->next)
		{
			if(strcmp(header->key, "RP-Application-Reason") == 0)
				response->error_code = (uint32_t)strtol(header->value, NULL, 0x10);
		}
		response->success = false;
	}
}


static bool session_thread_request_session(Ps4AppSession *session)
{
	int session_sock = -1;
	char host_buf[128];
	for(struct addrinfo *ai=session->connect_info.host_addrinfos; ai; ai=ai->ai_next)
	{
		if(ai->ai_protocol != IPPROTO_TCP)
			continue;

		struct sockaddr *sa = malloc(ai->ai_addrlen);
		if(!sa)
			continue;
		memcpy(sa, ai->ai_addr, ai->ai_addrlen);

		if(sa->sa_family == AF_INET)
			((struct sockaddr_in *)sa)->sin_port = htons(SESSION_PORT);
		else if(sa->sa_family == AF_INET6)
			((struct sockaddr_in6 *)sa)->sin6_port = htons(SESSION_PORT);
		else
		{
			free(sa);
			continue;
		}

		int r = getnameinfo(sa, ai->ai_addrlen, host_buf, sizeof(host_buf), NULL, 0, 0);
		if(r != 0)
		{
			free(sa);
			continue;
		}

		session_sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if(session_sock < 0)
			continue;
		r = connect(session_sock, sa, ai->ai_addrlen);
		if(r < 0)
		{
			if(errno == ECONNREFUSED)
				session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_CONNECTION_REFUSED;
			else
				session->quit_reason = PS4APP_QUIT_REASON_NONE;
			close(session_sock);
			session_sock = -1;
			free(sa);
			continue;
		}
		free(sa);

		session->connect_info.host_addrinfo_selected = ai;
		break;
	}


	if(session_sock < 0)
	{
		printf("Session Connection Failed.\n");
		if(session->quit_reason == PS4APP_QUIT_REASON_NONE)
			session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
		return false;
	}

	printf("Connected to %s:%u\n", host_buf, SESSION_PORT);

	static const char session_request_fmt[] =
			"GET /sce/rp/session HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"User-Agent: remoteplay Windows\r\n"
			"Connection: close\r\n"
			"Content-Length: 0\r\n"
			"RP-Registkey: %s\r\n"
			"Rp-Version: 8.0\r\n"
			"\r\n";

	char buf[512];
	int request_len = snprintf(buf, sizeof(buf), session_request_fmt,
							   host_buf, SESSION_PORT, session->connect_info.regist_key);
	if(request_len < 0 || request_len >= sizeof(buf))
	{
		close(session_sock);
		session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
		return false;
	}

	printf("sending\n%s\n", buf);

	ssize_t sent = send(session_sock, buf, (size_t)request_len, 0);
	if(sent < 0)
	{
		close(session_sock);
		session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
		return false;
	}

	size_t header_size;
	size_t received_size;
	Ps4AppErrorCode err = ps4app_recv_http_header(session_sock, buf, sizeof(buf), &header_size, &received_size);
	if(err != PS4APP_ERR_SUCCESS)
	{
		close(session_sock);
		session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
		return false;
	}

	buf[received_size] = '\0';
	printf("received\n%s\n", buf);

	Ps4AppHttpResponse http_response;
	err = ps4app_http_response_parse(&http_response, buf, header_size);
	if(err != PS4APP_ERR_SUCCESS)
	{
		close(session_sock);
		session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
		return false;
	}

	SessionResponse response;
	parse_session_response(&response, &http_response);

	if(response.success)
	{
		size_t nonce_len = PS4APP_KEY_BYTES;
		err = ps4app_base64_decode(response.nonce, strlen(response.nonce), session->nonce, &nonce_len);
		if(err != PS4APP_ERR_SUCCESS || nonce_len != PS4APP_KEY_BYTES)
		{
			response.success = false;
			session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
		}
	}
	else
	{
		switch(response.error_code)
		{
			case RP_APPLICATION_REASON_IN_USE:
				session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_RP_IN_USE;
				break;
			case RP_APPLICATION_REASON_CRASH:
				session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_RP_CRASH;
				break;
			default:
				session->quit_reason = PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN;
				break;
		}
	}

	ps4app_http_response_finish(&http_response);
	close(session_sock);
	return response.success;
}

