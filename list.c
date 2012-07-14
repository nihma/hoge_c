/* 
 * File:   list.c
 * Author: Makoto Nihei
 *
 * Created on 2012/07/12, 23:45
 */
#include "list.h"
#include "exception.h"

typedef struct list_cell ListCell;
struct list_cell {
  ListCell *pprev;
  ListCell *pnext;
  Var      *pv;
};

struct list_local {
  ListCell *pstart;
  ListCell *pend;
  ListCell *pcurrent;
  int      cell_num;
};

DECLARE_VAR_NEW(ListObject);

static void     List_delete(void *p);
static int      List_isEmpty(ListObject *pobj);
static void     List_setBegin(ListObject *pobj);
static int      List_hasCurrent(ListObject *pobj);
static void     List_setNext(ListObject *pobj);
static void     List_setPrev(ListObject *pobj);
static void     List_getCurrent(ListObject *pobj, Var **ppv);
static void     List_insertNext(ListObject *pobj, Var *pv);
static void     List_updateCurrent(ListObject *pobj, Var *pv);
static void     List_deleteCurrent(ListObject *pobj);
static int      List_count(ListObject *pobj);
static void     List_push(ListObject *pobj, Var *pv);
static void     List_pop(ListObject *pobj, Var **ppv);
static void     List_shift(ListObject *pobj, Var **ppv);
static void     List_unshift(ListObject *pobj, Var *pv);
static void     List_error(ListObject *pobj, char *str);

/* リストを初期化する */
VAR(ListObject) *List_new()
{
  ListLocal *plocal = (ListLocal *)malloc(sizeof(ListLocal));
  if (plocal == NULL) THROW(expeption_new("error: create list"));
  ListObject *pobj = (ListObject *)malloc(sizeof(ListObject));
  if (pobj == NULL) {
    free(plocal);
    THROW(expeption_new("error: create list"));
  }

  ListObject obj_def = {/* pl            = */ plocal,
                        /* count         = */ List_count,
                        /* push          = */ List_push,
                        /* pop           = */ List_pop,
                        /* shift         = */ List_shift,
                        /* unshift       = */ List_unshift,
                        /* isEmpty       = */ List_isEmpty,
                        /* setBegin      = */ List_setBegin,
                        /* hasCurrent    = */ List_hasCurrent,
                        /* setNext       = */ List_setNext,
                        /* setPrev       = */ List_setPrev,
                        /* getCurrent    = */ List_getCurrent,
                        /* insertNext    = */ List_insertNext,
                        /* updateCurrent = */ List_updateCurrent,
                        /* deleteCurrent = */ List_deleteCurrent};
  ListLocal local_def  = {/* pstart   = */ NULL,
                          /* pend     = */ NULL,
                          /* pcurrent = */ NULL,
                          /* cell_num = */ 0};
  *(obj_def.pl) = local_def;
  *pobj         = obj_def;

  return VAR_NEW(ListObject)(pobj, List_delete);
}

static void List_delete(void *p)
{
  ListObject *pobj = (ListObject *)p;
  if (pobj == NULL) return;
  if (pobj->pl == NULL) {
    free(pobj);
    return;
  }

  ListCell *pcell;
  while (pobj->pl->pend != NULL) {
    pobj->pl->pend->pv->end(&(pobj->pl->pend->pv));
    pcell = pobj->pl->pend;
    pobj->pl->pend = pobj->pl->pend->pprev;
    free(pcell);
  }
  free(pobj->pl);
  free(pobj);
}

/* リストが空かどうか調べる */
/* 空なら1,殻でなければ0を返す */
static int List_isEmpty(ListObject *pobj)
{
	return pobj->pl == NULL || pobj->pl->cell_num == 0;
}

static void List_setBegin(ListObject *pobj)
{
  pobj->pl->pcurrent = pobj->pl->pstart;
}

static int List_hasCurrent(ListObject *pobj)
{
  return pobj->pl->pcurrent != NULL;
}

static void List_setNext(ListObject *pobj)
{
  if (! List_hasCurrent(pobj)) List_error(pobj, "error: no value");

  pobj->pl->pcurrent = pobj->pl->pcurrent->pnext;
}

static void List_setPrev(ListObject *pobj)
{
  if (! List_hasCurrent(pobj)) List_error(pobj, "error: no value");

  pobj->pl->pcurrent = pobj->pl->pcurrent->pprev;
}

static void List_getCurrent(ListObject *pobj, Var **ppv)
{
  if (! List_hasCurrent(pobj)) List_error(pobj, "error: no value");

  *ppv = pobj->pl->pcurrent->pv->clone(pobj->pl->pcurrent->pv);
}

static void List_insertNext(ListObject *pobj, Var *pv)
{
  ListCell *pcell = (ListCell *)malloc(sizeof(struct list_cell));
  if (pcell == NULL) List_error(pobj, "error: create cell");

  pcell->pv = pv->clone(pv);

  pcell->pprev = pobj->pl->pcurrent;
  pcell->pnext = pobj->pl->pcurrent->pnext;
  if (pcell->pprev != NULL) pcell->pprev->pnext = pcell;

  pobj->pl->pcurrent = pcell;
  pobj->pl->cell_num++;
  if (pobj->pl->cell_num == 1) pobj->pl->pstart = pobj->pl->pend = pcell;
}

static void List_updateCurrent(ListObject *pobj, Var *pv)
{
	if (! List_hasCurrent(pobj)) List_error(pobj, "error: no value");

  pobj->pl->pcurrent->pv->end(&(pobj->pl->pcurrent->pv));
  pobj->pl->pcurrent->pv = pv->clone(pv);
}

static void List_deleteCurrent(ListObject *pobj)
{
	if (! List_hasCurrent(pobj)) List_error(pobj, "error: no value");

  (pobj->pl->pcurrent)->pv->end(&((pobj->pl->pcurrent)->pv));
	ListCell *pcell = pobj->pl->pcurrent;

	if (pcell->pprev != NULL) pcell->pprev->pnext = pcell->pnext;
	if (pcell->pnext != NULL) pcell->pnext->pprev = pcell->pprev;
	free(pcell);

	pobj->pl->cell_num--;
	if (pobj->pl->cell_num == 0) pobj->pl->pstart = pobj->pl->pend = NULL;
}

static int List_count(ListObject *pobj)
{
	return pobj->pl->cell_num;
}

/* リストにデータをつむ */
static void List_push(ListObject *pobj, Var *pv)
{
	ListCell *pcell = (ListCell *)malloc(sizeof(struct list_cell));
	if (pcell == NULL) List_error(pobj, "error: create cell");

	pcell->pv = pv->clone(pv);
	pcell->pprev = pobj->pl->pend;
	pcell->pnext = NULL;
	if (pcell->pprev != NULL) pcell->pprev->pnext = pcell;

	pobj->pl->pend = pcell;
	pobj->pl->cell_num++;
	if (pobj->pl->cell_num == 1) pobj->pl->pstart = pcell;
}

/* リストからデータをおろす */
static void List_pop(ListObject *pobj, Var **ppv)
{
	if (List_isEmpty(pobj)) List_error(pobj, "error: stack underflow");

  *ppv = pobj->pl->pend->pv;

	ListCell *pcell = pobj->pl->pend;
	pobj->pl->pend = pobj->pl->pend->pprev;
	if (pobj->pl->pcurrent == pcell) pobj->pl->pcurrent = NULL;
	free(pcell);
	if (pobj->pl->pend != NULL) pobj->pl->pend->pnext = NULL;

	pobj->pl->cell_num--;
	if (pobj->pl->cell_num == 0) pobj->pl->pstart = NULL;
}

/* リストの先頭からデータを取り出す */
static void List_shift(ListObject *pobj, Var **ppv)
{
	if (List_isEmpty(pobj)) List_error(pobj, "error: stack underflow");

  *ppv = pobj->pl->pstart->pv;

	ListCell *pcell = pobj->pl->pstart;
	pobj->pl->pstart = pobj->pl->pstart->pnext;
	if (pobj->pl->pcurrent == pcell) pobj->pl->pcurrent = NULL;
	free(pcell);
	if (pobj->pl->pstart != NULL) pobj->pl->pstart->pprev = NULL;

	pobj->pl->cell_num--;
	if (pobj->pl->cell_num == 0) pobj->pl->pend = NULL;
}

/* リストの先頭にデータを加える */
static void List_unshift(ListObject *pobj, Var *pv)
{
	ListCell *pcell = (ListCell *)malloc(sizeof(struct list_cell));
	if (pcell == NULL) List_error(pobj, "error: create cell");

	pcell->pv = pv->clone(pv);

	pcell->pprev = NULL;
	pcell->pnext = pobj->pl->pstart;
	if (pcell->pnext != NULL) pcell->pnext->pprev = pcell;

	pobj->pl->pstart = pcell;
	pobj->pl->cell_num++;
	if (pobj->pl->cell_num == 1) pobj->pl->pend = pcell;
}

/* 内部関数 */
static void List_error(ListObject *pobj, char *str)
{
  List_delete(pobj);
  THROW(expeption_new(str));
}

