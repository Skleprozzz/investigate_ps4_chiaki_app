#include <ps4app/ctrl.h>
#include <ps4app/session.h>
#include <ps4app/base64.h>
#include <ps4app/http.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define SESSION_CTRL_PORT 9295

static void *ctrl_thread_func(void *user);

PS4APP_EXPORT Ps4AppErrorCode ps4app_ctrl_start(Ps4AppCtrl *ctrl, Ps4AppSession *session)
{
	ctrl->session = session;
	return ps4app_thread_create(&ctrl->thread, ctrl_thread_func, ctrl);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_ctrl_join(Ps4AppCtrl *ctrl)
{
	return ps4app_thread_join(&ctrl->thread, NULL);
}

static Ps4AppErrorCode ctrl_thread_connect(Ps4AppCtrl *ctrl);

static void *ctrl_thread_func(void *user)
{
	Ps4AppCtrl *ctrl = user;
	Ps4AppErrorCode err = ctrl_thread_connect(ctrl);
	if (err != PS4APP_ERR_SUCCESS)
	{
		ps4app_session_set_quit_reason(ctrl->session, PS4APP_QUIT_REASON_CTRL_UNKNOWN);
		return NULL;
	}

	return NULL;
}

typedef struct ctrl_response_t
{
	bool server_type_valid;
	uint8_t rp_server_type[0x10];
	bool success;
} CtrlResponse;

static void parse_ctrl_response(CtrlResponse *response, Ps4AppHttpResponse *http_response)
{
	memset(response, 0, sizeof(CtrlResponse));

	if (http_response->code != 200)
	{
		response->success = false;
		return;
	}

	response->success = true;
	response->server_type_valid = false;
	for (Ps4AppHttpHeader *header = http_response->headers; header; header = header->next)
	{
		if (strcmp(header->key, "RP-Server-Type") == 0)
		{
			size_t server_type_size = sizeof(response->rp_server_type);
			ps4app_base64_decode(header->value, strlen(header->value) + 1, response->rp_server_type, &server_type_size);
			response->server_type_valid = server_type_size == sizeof(response->rp_server_type);
		}
	}
}

static Ps4AppErrorCode ctrl_thread_connect(Ps4AppCtrl *ctrl)
{
	Ps4AppSession *session = ctrl->session;
	struct addrinfo *addr = session->connect_info.host_addrinfo_selected;
	struct sockaddr *sa = malloc(addr->ai_addrlen);
	if (!sa)
		return PS4APP_ERR_MEMORY;
	memcpy(sa, addr->ai_addr, addr->ai_addrlen);

	if (sa->sa_family == AF_INET)
		((struct sockaddr_in *)sa)->sin_port = htons(SESSION_CTRL_PORT);
	else if (sa->sa_family == AF_INET6)
		((struct sockaddr_in6 *)sa)->sin6_port = htons(SESSION_CTRL_PORT);
	else
		return PS4APP_ERR_INVALID_DATA;

	int sock = socket(sa->sa_family, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{
		PS4APP_LOGE(&session->log, "Session ctrl socket creation failed.\n");
		if (session->quit_reason == PS4APP_QUIT_REASON_NONE)
			session->quit_reason = PS4APP_QUIT_REASON_CTRL_UNKNOWN;
		return PS4APP_ERR_NETWORK;
	}

	PS4APP_LOGI(&session->log, "Connected to %s:%d\n", session->connect_info.hostname, SESSION_CTRL_PORT);

	static const char request_fmt[] =
		"GET /sce/rp/session/ctrl HTTP/1.1\r\n"
		"Host: %s:%d\r\n"
		"User-Agent: remoteplay Windows\r\n"
		"Connection: keep-alive\r\n"
		"Content-Length: 0\r\n"
		"RP-Auth: %s\r\n"
		"RP-Version: 8.0\r\n"
		"RP-Did: %s\r\n"
		"RP-ControllerType: 3\r\n"
		"RP-ClientType: 11\r\n"
		"RP-OSType: %s\r\n"
		"RP-ConPath: 1\r\n\r\n";

	close(sock);
	return PS4APP_ERR_SUCCESS;
}