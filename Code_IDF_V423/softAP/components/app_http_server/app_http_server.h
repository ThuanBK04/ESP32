#ifndef __APP_HTTP_SERVER_H
#define __APP_HTTP_SERVER_H

typedef void (*http_server_handle) (char * data, int len);

void start_webserver(void);
void stop_webserver(void);
void http_get_set_callback (void * cb);
void http_post_set_callback (void * cb);
void httpd_send_response(buf, buf_len);
void rgb_post_set_callback (void * cb);

#endif