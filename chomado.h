#ifndef _CHOMADO_H_
#define _CHOMADO_H_

/*------------------------------------*/
/* include */
/*------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iutil.h>
#include <ilist.h>

/*------------------------------------*/
/* define */
/*------------------------------------*/
#define N 10  /* # problems */

/*------------------------------------*/
/* ui */
/*------------------------------------*/
typedef unsigned int ui;
ui *ui_new(ui _v);
ui *ui_new_ary(ilist *_a);

/*------------------------------------*/
/* search tree */
/*------------------------------------*/
typedef struct NODE node;
struct NODE
{
  node *ch[N+1];  /* children */
  ui nc;          /* # candidates */
  ui *c;          /* candidattes */
  ui x;           /* tested pattern */
};
node *node_new(void);
void  node_free(node *_n);
node *node_new_root(void);
void  node_expand_at(node *_n, int _s);
void  node_expand(node *_n);
ui    node_depth(node *_n);
ui    node_nleaf(node *_n);

/*------------------------------------*/
/* utils */
/*------------------------------------*/
ui     chomado_get(ui _a, int _i);
void   chomado_print(ui _a);
int    chomado_compare(ui _a, ui _b);
double chomado_entropy(int *_h);
double chomado_wsi(int *_h);

/*------------------------------------*/
/* for search */
/*------------------------------------*/
void   chomado_init(void);
ilist *chomado_update(int _nc, ui *_c, ui _x, int _s);
ui     chomado_select(int _nc, ui *_c);

/*------------------------------------*/
/* search main */
/*------------------------------------*/
void chomado_search(void);

#endif
