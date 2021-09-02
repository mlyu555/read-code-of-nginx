
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


/**
 * @brief 创建数组结构对象
 *
 * @param p 被分配的内存池对象
 * @param n 可用元素个数
 * @param size  单个元素大小
 * @return ngx_array_t* 数组对象
 */
ngx_array_t *
ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;

    // 分配内存
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }

    // 初始化
    if (ngx_array_init(a, p, n, size) != NGX_OK) {
        return NULL;
    }

    return a;
}


/**
 * @brief 销毁数组指针(仅刚在内存池上分配才可销毁——并未释放内存而是标记可重用)
 *
 * @param a 数组指针
 */
void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

    // 内存池上清除数组元素所占内存
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

    // 内存池上清除数组结构所占内存
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}


/**
 * @brief   数组添加元素(使用该地址进行赋值即可)
 *
 * @param a         数组对象
 * @return void*    待添加元素的地址，有且仅当无法申请内存时返回NULL
 */
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

    // 数组已满
    // 若原数组元素所在的内存块内还有空间则每次扩容1个元素大小；否则重新申请内存块，扩容1倍
    if (a->nelts == a->nalloc) {

        /* the array is full */

        size = a->size * a->nalloc;     // 原数组元素区大小

        p = a->pool;

        // 该数组元素区是否可连续分配（可以则扩容1个）
        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array */

            // 扩容1倍
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

    // 首地址+已用内存
    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}


/**
 * @brief   数组添加多个元素(res[0]来赋值)
 *
 * @param a 数组对象
 * @param n 新增元素个数
 * @return void*  待添加元素集的首地址(数组指针)，有且仅当无法申请内存时返回NULL
 */
void *
ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;     // 待增加元素所需内存大小

    if (a->nelts + n > a->nalloc) {

        /* the array is full */

        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */

            // 动态扩容，若新增元素个数n大于数组大小，则扩容为2n，否则为2nalloc
            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new = ngx_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, a->nelts * a->size);
            a->elts = new;
            a->nalloc = nalloc;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}

