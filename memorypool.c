#include <stdio.h>
#include <stdlib.h>

#include <ngx_core.h>
#include <ngx_palloc.h>

int main() {
    printf("sizeof(ngx_pool_t)                = %d\n", sizeof(ngx_pool_t));
    printf("sizeof(ngx_pool_data_t)           = %d\n", sizeof(ngx_pool_data_t));
    printf("sizeof(ngx_pool_large_t)          = %d\n", sizeof(struct ngx_pool_large_s));


    return 0;
}