#include <app/session.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char session_request[] =
	"GET /sce/rp/session HTTP/1.1\r\n"
	"Host: 192.168.  1.  8:9295\r\n"
	"User-Agent: remoteplay Windows\r\n"
	"Connection: close\r\n"
	"Content-Length: 0\r\n"
	"RP-Registkey: 3131633065363864\r\n"
	"Rp-Version: 8.0\r\n"
	"\r\n";

static void *session_thread_func(void *arg);

APP_EXPORT void app_session_init(AppSession *session, AppConnectInfo *connect_info)
{
	session->connect_info.host = strdup(connect_info->regist_key);
	session->connect_info.regist_key = strdup(connect_info->regist_key);
	session->connect_info.ostype = strdup(connect_info->ostype);
	memcpy(session->connect_info.auth, connect_info->auth, sizeof(session->connect_info.auth));
	memcpy(session->connect_info.morning, connect_info->morning, sizeof(session->connect_info.morning));
}

APP_EXPORT bool app_session_finish(AppSession *session)
{
	free(session->connect_info.host);
	free(session->connect_info.regist_key);
	free(session->connect_info.ostype);
}

APP_EXPORT bool App_session_start(AppSession *session)
{
	bool r = app_thread_create(&session->session_thread, session_thread_func, session);
	if (!r)
		return false;
	return true;
}

APP_EXPORT void app_session_join(AppSession *session)
{
	app_thread_join(&session->session_thread, NULL);
}

static void *session_thread_func(void *arg)
{
	AppSession *session = arg;
	printf("Hello World'n");
	return NULL;
}
