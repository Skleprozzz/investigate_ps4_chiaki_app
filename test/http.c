#include <munit.h>

#include <ps4app/http.h>

static const char *response =
		"HTTP/1.1 200 OK\r\n"
		"Content-type: text/html, text, plain\r\n"
		"\r\n";

static void *test_http_response_parse_setup(const MunitParameter params[], void *user)
{
	return strdup(response);
}

static void test_http_response_parse_teardown(void *fixture)
{
	free(fixture);
}

static MunitResult test_http_response_parse(const MunitParameter params[], void *fixture)
{
	char *buf = fixture;
	Ps4AppHttpResponse parsed_response;
	Ps4AppErrorCode err = ps4app_http_response_parse(&parsed_response, buf, strlen(buf));
	munit_assert_int(err, ==, PS4APP_ERR_SUCCESS);
	munit_assert_int(parsed_response.code, ==, 200);
	return MUNIT_OK;
}

MunitTest tests_http[] = {
	{
		"/response_parse",
		test_http_response_parse,
		test_http_response_parse_setup,
		test_http_response_parse_teardown,
		MUNIT_TEST_OPTION_NONE,
		NULL
	},
	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};