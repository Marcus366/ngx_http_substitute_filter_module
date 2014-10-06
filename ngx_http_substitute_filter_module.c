
/*
 * Copyright (C) Marcus Zeng
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_str_t old;
    ngx_str_t new;
} ngx_http_substitute_loc_conf_t;


static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;


static void *ngx_http_substitute_create_loc_conf(ngx_conf_t *cf);
static ngx_int_t ngx_http_substitute_init(ngx_conf_t *cf);
static char* ngx_http_substitute_set(ngx_conf_t *cf, ngx_command_t *command, void *conf);
static ngx_int_t ngx_http_substitute_header_filter(ngx_http_request_t *r);
static ngx_int_t ngx_http_substitute_body_filter(ngx_http_request_t *r, ngx_chain_t *in);


static ngx_command_t  ngx_http_substitute_commands[] = {
    
    { ngx_string("substitute"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE12,
      ngx_http_substitute_set,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


ngx_http_module_t  ngx_http_substitute_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_substitute_init,              /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_substitute_create_loc_conf,   /* create location configuration */
    NULL                                   /* merge location configuration */
};
 

ngx_module_t  ngx_http_substitute_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_substitute_module_ctx,       /* module context */
    ngx_http_substitute_commands,          /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t ngx_http_substitute_header_filter(ngx_http_request_t *r)
{
    return ngx_http_next_header_filter(r);
}


static ngx_int_t ngx_http_substitute_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_http_substitute_loc_conf_t *conf;
    ngx_buf_t *b;
    ngx_chain_t *cl;

    conf = ngx_http_get_module_loc_conf(r, ngx_http_substitute_filter_module);

    b = ngx_create_temp_buf(r->pool, 10);
    b->pos = b->start = conf->old.data;
    b->last = b->pos + conf->old.len;

    cl = ngx_alloc_chain_link(r->pool);
    cl->buf = b;
    cl->next = in;

    return ngx_http_next_body_filter(r, cl);
}

static void* ngx_http_substitute_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_substitute_loc_conf_t *conf;

    conf = (ngx_http_substitute_loc_conf_t*) ngx_pcalloc(cf->pool, 
                                    sizeof(ngx_http_substitute_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    /*
     * auto initilize by ngx_pcalloc.
     * conf->old = {NULL, 0};
     * conf->new = {NULL, 0};
     */

    return conf;
}


static ngx_int_t ngx_http_substitute_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter  = ngx_http_substitute_header_filter;

    ngx_http_next_body_filter   = ngx_http_top_body_filter;
    ngx_http_top_body_filter    = ngx_http_substitute_body_filter;

    return NGX_OK;
}


static char* ngx_http_substitute_set(ngx_conf_t *cf, ngx_command_t *command, void *conf)
{
    ngx_http_substitute_loc_conf_t *subconf = (ngx_http_substitute_loc_conf_t*) conf;

    ngx_str_t *value = cf->args->elts;

    if (cf->args->nelts > 1) {
        subconf->old = value[1];
    }

    if (cf->args->nelts > 2) {
        subconf->new = value[2];   
    }

    return NGX_CONF_OK;
}

