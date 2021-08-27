#include <stdio.h>
#include <stdlib.h>

#include <ngx_core.h>

typedef struct {
    void        **main_conf;    // 数组指针，数组元素为create_main_conf产生的结构体
    void        **srv_conf;     // create_srv_conf
    void        **loc_conf;     // create_loc_conf
} ngx_http_conf_ctx_t;

int main() {
    printf("sizeof(unsigned)                = %d\n", sizeof(unsigned));
    printf("sizeof(void*)                   = %d\n", sizeof(void *));
    printf("sizeof(ngx_http_conf_ctx_t)     = %d\n", sizeof(ngx_http_conf_ctx_t));

    printf("sizeof(ngx_uint_t)                = %d\n", sizeof(ngx_uint_t));
    printf("sizeof(u_short)                = %d\n", sizeof(u_short));
    printf("sizeof(u_char)                = %d\n", sizeof(u_char));


    return 0;
}