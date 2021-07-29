#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

// 回调函数
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r);
static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_http_module_t ngx_http_mytest_module_ctx = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL};

static ngx_command_t ngx_http_mytest_commands[] = {
    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        ngx_http_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL,
    },
    ngx_null_command};

ngx_module_t ngx_http_mytest_module = {
    NGX_MODULE_V1,
    &ngx_http_mytest_module_ctx,
    ngx_http_mytest_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING};

// 处理nginx.conf配置项参数
static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;

    // 获取 mytest 配置项所属的配置块
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    // 如果请求的主机域名、URI与mytest配置项所在的配置块相匹配，调用该方法
    clcf->handler = ngx_http_mytest_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
    // 必须是 GET 获得 HEAD 方法
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD)))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }

    // 丢弃请求中的包体
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK)
    {
        return rc;
    }

    ngx_str_t type = ngx_string("text/plain");
    ngx_str_t response = ngx_string("Hello mytest Module!\n");
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type = type;

    // 发送HTTP头部
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
    {
        return rc;
    }

    // 构造包体
    ngx_buf_t *buf;
    buf = ngx_create_temp_buf(r->pool, response.len);
    if (buf == NULL)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_memcpy(buf->pos, response.data, response.len);
    buf->last = buf->pos + response.len;
    buf->last_buf = 1; // ?

    // 构造发送时的 ngx_chain_t 结构体
    ngx_chain_t out;
    out.buf = buf;
    out.next = NULL;

    // 发送包体
    return ngx_http_output_filter(r, &out);
}