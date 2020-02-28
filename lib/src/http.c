#include <ps4app/http.h>

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>


PS4APP_EXPORT void ps4app_http_header_free(Ps4AppHttpHeader *header)
{
	while(header)
	{
		Ps4AppHttpHeader *cur = header;
		header = header->next;
		free(cur);
	}
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_http_header_parse(Ps4AppHttpHeader **header, char *buf, size_t buf_size)
{
	*header = NULL;
#define FAIL(reason) do { ps4app_http_header_free(*header); return (reason); } while(0);
	char *key_ptr = buf;
	char *value_ptr = NULL;

	for(char *end = buf + buf_size; buf<end; buf++)
	{
		char c = *buf;
		if(!c)
			break;

		if(!value_ptr)
		{
			if(c == ':')
			{
				if(key_ptr == buf)
					FAIL(PS4APP_ERR_INVALID_DATA);
				*buf = '\0';
				buf++;
				if(buf == end || *buf != ' ')
					FAIL(PS4APP_ERR_INVALID_DATA);
				buf++;
				if(buf == end)
					FAIL(PS4APP_ERR_INVALID_DATA);
				value_ptr = buf;
			}
			else if(c == '\r' || c == '\n')
			{
				if(key_ptr + 1 < buf) // no : encountered yet
					FAIL(PS4APP_ERR_INVALID_DATA);
				key_ptr = buf + 1;
			}
		}
		else
		{
			if(c == '\r' || c == '\n')
			{
				if(value_ptr == buf) // empty value
					FAIL(PS4APP_ERR_INVALID_DATA);

				*buf = '\0';
				Ps4AppHttpHeader *entry = malloc(sizeof(Ps4AppHttpHeader));
				if(!entry)
					FAIL(PS4APP_ERR_MEMORY);
				entry->key = key_ptr;
				entry->value = value_ptr;
				entry->next = *header;
				*header = entry;

				key_ptr = buf + 1;
				value_ptr = NULL;
			}
		}
	}
	return PS4APP_ERR_SUCCESS;
#undef FAIL
}

PS4APP_EXPORT void ps4app_http_response_finish(Ps4AppHttpResponse *response)
{
	if(!response)
		return;
	ps4app_http_header_free(response->headers);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_http_response_parse(Ps4AppHttpResponse *response, char *buf, size_t buf_size)
{
	static const char *http_version = "HTTP/1.1 ";
	static const size_t http_version_size = 9;

	if(buf_size < http_version_size)
		return PS4APP_ERR_INVALID_DATA;

	if(strncmp(buf, http_version, http_version_size) != 0)
		return PS4APP_ERR_INVALID_DATA;

	buf += http_version_size;
	buf_size -= http_version_size;

	char *line_end = memchr(buf, '\r', buf_size);
	if(!line_end)
		return PS4APP_ERR_INVALID_DATA;
	size_t line_length = (line_end - buf) + 2;
	if(buf_size <= line_length || line_end[1] != '\n')
		return PS4APP_ERR_INVALID_DATA;
	*line_end = '\0';

	char *endptr;
	response->code = (int)strtol(buf, &endptr, 10);
	if(response->code == 0)
		return PS4APP_ERR_INVALID_DATA;

	buf += line_length;
	buf_size -= line_length;

	return ps4app_http_header_parse(&response->headers, buf, buf_size);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_recv_http_header(int sock, char *buf, size_t buf_size, size_t *header_size, size_t *received_size)
{
	// 0 = ""
	// 1 = "\r"
	// 2 = "\r\n"
	// 3 = "\r\n\r"
	// 4 = "\r\n\r\n" (final)
	int nl_state = 0;
	static const int transitions_r[] = { 1, 1, 3, 1 };
	static const int transitions_n[] = { 0, 2, 0, 4 };

	*received_size = 0;
	while(true)
	{
		ssize_t received = recv(sock, buf, buf_size, 0);
		if(received <= 0)
			return PS4APP_ERR_NETWORK;

		*received_size += received;
		for(; received > 0; buf++, received--)
		{
			switch(*buf)
			{
				case '\r':
					nl_state = transitions_r[nl_state];
					break;
				case '\n':
					nl_state = transitions_n[nl_state];
					break;
				default:
					nl_state = 0;
					break;
			}
			if(nl_state == 4){
				received--;
				break;
			}
		}

		if(nl_state == 4)
		{
			*header_size = *received_size - received;
			break;
		}
	}

	return PS4APP_ERR_SUCCESS;
}