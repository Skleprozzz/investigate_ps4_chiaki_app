#include <ps4app/ctrl.h>
#include <ps4app/session.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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



	return NULL;
}

static Ps4AppErrorCode ctrl_thread_connect(Ps4AppCtrl *ctrl)
{
	Ps4AppSession *session = ctrl->session;
	struct addrinfo *addr = session->connect_info.host_addrinfo_selected;
	struct sockaddr *sa = malloc(addr->ai_addrlen);
	if(!sa)
		return PS4APP_ERR_MEMORY;
	memcpy(sa, addr->ai_addr, addr->ai_addrlen);

	if(sa->sa_family == AF_INET)
		((struct sockaddr_in *)sa)->sin_port = htons(SESSION_CTRL_PORT);
	else if(sa->sa_family == AF_INET6)
		((struct sockaddr_in6 *)sa)->sin6_port = htons(SESSION_CTRL_PORT);
	else
		return PS4APP_ERR_INVALID_DATA;

	int sock = socket(sa->sa_family, SOCK_STREAM, IPPROTO_TCP);
	if(sock < 0)
	{
		PS4APP_LOGE(&session->log, "Session ctrl socket creation failed.\n");
		if(session->quit_reason == PS4APP_QUIT_REASON_NONE)
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