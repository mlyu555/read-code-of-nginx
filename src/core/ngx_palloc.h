
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;      // 清理回调函数
    void                 *data;         // 待清理数据内存
    ngx_pool_cleanup_t   *next;         // 链表组织
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s {
    ngx_pool_large_t     *next;     // 下一数据块
    void                 *alloc;    // 数据块指针
};


typedef struct {
    u_char               *last;     // 可用内存开始指针地址
    u_char               *end;      // 结束地址
    ngx_pool_t           *next;     // 内存池下一结点
    ngx_uint_t            failed;   // 失败次数
} ngx_pool_data_t;

// typedef struct ngx_pool_s        ngx_pool_t;
struct ngx_pool_s {
    ngx_pool_data_t       d;            // 数据域
    size_t                max;          // 最大可分配内存
    ngx_pool_t           *current;      // 当前内存池指针地址
    ngx_chain_t          *chain;        // 缓冲区链表
    ngx_pool_large_t     *large;        // 大块数据链表
    ngx_pool_cleanup_t   *cleanup;      // 自定义回调函数，清除分配内存
    ngx_log_t            *log;          // 日志
};


typedef struct {
    ngx_fd_t              fd;
    u_char               *name;
    ngx_log_t            *log;
} ngx_pool_cleanup_file_t;

// --- API ---

// 创建内存池
ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
// 销毁内存池
void ngx_destroy_pool(ngx_pool_t *pool);
// 重置内存池
void ngx_reset_pool(ngx_pool_t *pool);
// 分配内存，对齐
void *ngx_palloc(ngx_pool_t *pool, size_t size);
// 分配内存，不对齐
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);
// 分配内存，对齐并初始化为0
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
// 分配内存，指定对齐大小（挂载在large上）
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
// 释放large内存
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);


// --- Nginx cleanup机制 ---

// 添加cleanup节点，并分配内存但未添加实际数据
ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);
// 清除cleanup链表上的内存块（主要是fd）
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);
// callback 关闭文件描述符 等价 close(fd)
void ngx_pool_cleanup_file(void *data);
// callback 删除文件 等价unlink(filename)
void ngx_pool_delete_file(void *data);


#endif /* _NGX_PALLOC_H_INCLUDED_ */
