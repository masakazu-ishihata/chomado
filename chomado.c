#include "chomado.h"

#define max(a, b) ( a < b ? b : a )
#define min(a, b) ( a > b ? b : a )

/*----------------------------------------------------------------------------*/
/* unsigned int */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* create a new ui pointer */
/*------------------------------------*/
ui *ui_new(ui _v)
{
  ui *p = (ui *)imalloc(sizeof(ui), "");
  *p = _v;
  return p;
}
/*------------------------------------*/
/* translate ilist -> ui ary */
/*------------------------------------*/
ui *ui_new_ary(ilist *_a)
{
  int l = ilist_size(_a), i;
  ui *a = (ui *)imalloc(l * sizeof(ui), ""), *p;
  for(i=0, p=(ui *)ilist_head(_a); p!=NULL; a[i]=*p, i++, p=(ui *)ilist_succ(_a));
  return a;
}


/*----------------------------------------------------------------------------*/
/* for search */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* initialize mask for evaluations */
/*------------------------------------*/
int mask[N]; /* mask */
void chomado_init(void)
{
  int i;
  ui m;
  for(i=0, m=3; i<N; mask[i]=m, i++, m=m<<2);
}
/*------------------------------------*/
/* update candidates c */
/*------------------------------------*/
ilist *chomado_update(int _nc, ui *_c, ui _x, int _s)
{
  int i;
  ilist *c = ilist_new();

  for(i=0; i<_nc; i++)
    if(chomado_compare(_x, _c[i]) == _s) ilist_push(c, ui_new(_c[i]));

  return c;
}
/*------------------------------------*/
/* find the best solutiion */
/*------------------------------------*/
ui chomado_select(int _nc, ui *_c)
{
  ui i, x, max_x = 0;
  double e, max_e = 0;
  int h[N + 1];

  /* only one candidate */
  if(_nc == 1) return _c[0];

  /* select the best */
  for(x=0; x<pow(4, N); x++){
    /* init histgram */
    for(i=0; i<=N; h[i]=0, i++);

    /* get histgram */
    for(i=0; i<_nc; h[ chomado_compare(x, _c[i]) ]++, i++);

    /* entropy */
    //    e = chomado_entropy(h);
    e = chomado_wsi(h);
    if(max_e < e){
      max_x = x;
      max_e = e;
    }
  }
  return max_x;
}


/*----------------------------------------------------------------------------*/
/* search tree */
/*----------------------------------------------------------------------------*/
ui node_num = 0;
/*------------------------------------*/
/* create a new node */
/*------------------------------------*/
node *node_new(void)
{
  node *n;
  int i;

  n = (node *)imalloc(sizeof(node), "");
  for(i=0; i<=N; n->ch[i]=NULL, i++);
  n->id = node_num++;
  n->nc = 0;
  n->c  = NULL;
  n->x  = 0;

  return n;
}
/*------------------------------------*/
/* free */
/*------------------------------------*/
void node_free(node *_n)
{
  int i;
  if(_n != NULL){
    for(i=0; i<=N; i++) node_free(_n->ch[i]);
    free(_n->c);
    free(_n);
  }
}
/*------------------------------------*/
/* create a new root node */
/*------------------------------------*/
node *node_new_root(void)
{
  node *n = node_new();
  int i;

  n->nc = pow(4, N);
  n->c  = (ui *)imalloc(n->nc * sizeof(ui), "");
  for(i=0; i<n->nc; n->c[i]=i, i++);
  n->x  = 0;

  return n;
}
/*------------------------------------*/
/* expand the _s-th child of node _n */
/*------------------------------------*/
void node_expand_at(node *_n, int _s)
{
  node *n;
  ilist *c;

  c = chomado_update(_n->nc, _n->c, _n->x, _s);

  if(ilist_size(c) > 0){
    n = node_new();
    n->nc = (int)ilist_size(c);
    n->c  = ui_new_ary(c);
    n->x  = chomado_select(n->nc, n->c);
    _n->ch[_s] = n;
  }

  ilist_free_func(c, free);
}
/*------------------------------------*/
/* expand all children of _n */
/*------------------------------------*/
void node_expand(node *_n)
{
  int i;
  if(_n->nc > 1)
    for(i=0; i<=N; i++)
      node_expand_at(_n, i);
}
/*------------------------------------*/
/* the depth of tree _n */
/*------------------------------------*/
ui node_depth(node *_n)
{
  ui i, d = 0;

  if(_n == NULL) return 0;
  for(i=0; i<=N; i++) d = max(d, node_depth(_n->ch[i]));
  return d + 1;
}
/*------------------------------------*/
/* # leaves in tree _n */
/*------------------------------------*/
ui node_nleaf(node *_n)
{
  ui i, n = 0;
  if(_n == NULL)  return 0;
  for(i=0; i<=N; i++) n += node_nleaf(_n->ch[i]);
  if(n == 0)  return 1;
  else        return n;
}
/*------------------------------------*/
/* show */
/*------------------------------------*/
void node_show(node *_n)
{
  int i;

  printf("Node %u\n", _n->id);
  printf("Try "); chomado_print(_n->x);
  printf("Go  ");
  for(i=0; i<=N; i++)
    if(_n->ch[i] != NULL) printf(" %d:%u", i, (_n->ch[i])->id);
  printf("\n");
}

/*----------------------------------------------------------------------------*/
/* utils */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* get the i-th value */
/*------------------------------------*/
ui chomado_get(ui _a, int _i)
{
  return (_a & mask[_i]) >> (2*_i);
}
/*------------------------------------*/
/* print solution */
/*------------------------------------*/
void chomado_print(ui _a)
{
  int i;
  for(i=0; i<N; i++) printf("%2d", chomado_get(_a, N - i - 1) );
  printf("\n");
}
/*------------------------------------*/
/* compare _a & _b */
/*------------------------------------*/
int chomado_compare(ui _a, ui _b)
{
  int i, c=_a^_b, s=0;
  for(i=0; i<N; i++) if( !(c & mask[i]) ) s++;
  return s;
}
/*------------------------------------*/
/* entropy */
/*------------------------------------*/
double chomado_entropy(int *_h)
{
  int i;
  double s=0, e=0;

  for(i=0; i<=N; s+=_h[i], i++);
  for(i=0; i<=N; i++)
    if(_h[i] > 0) e += -(_h[i]/s) * log(_h[i]/s);
  return e;
}
/*------------------------------------*/
/* worst self information */
/*------------------------------------*/
double chomado_wsi(int *_h)
{
  int i;
  double s=0, e=(N+1);

  for(i=0; i<=N; s+=_h[i], i++);
  for(i=0; i<=N; i++)
    if(_h[i] > 0) e = min(e, -log(_h[i]/s));

  return e;
}

/*----------------------------------------------------------------------------*/
/* search main */
/*----------------------------------------------------------------------------*/
void chomado_search(void)
{
  chomado_init();

  int i;
  node *n, *r = node_new_root();
  ilist *nl = ilist_new();
  double t1, t2;

  t1 = itime_get();

  ilist_push(nl, r);

  while((n = ilist_shift(nl)) != NULL){
    /* expand */
    node_expand(n);
    for(i=0; i<=N; i++)
      if(n->ch[i] != NULL) ilist_push(nl, n->ch[i]);

    /* current node */
    node_show(n);
  }

  t2 = itime_get();

  /* result */
  printf("N = %2d : |A| = %7u, W = %d, T = %e\n", N, node_nleaf(r), node_depth(r)-1, t2-t1);

  node_free(r);
}
