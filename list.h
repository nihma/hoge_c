/* 
 * File:   list.h
 * Author: Makoto Nihei
 *
 * Created on 2012/07/08, 23:45
 */

#ifndef LIST_H
#define	LIST_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "var.h"

typedef struct list_local  ListLocal;
typedef struct list_object ListObject;
DECLARE_VAR_TYPE(ListObject);
struct list_object {
  ListLocal *pl;
  int  (*count)(ListObject *pobj);
  void (*push)(ListObject *pobj, Var *pv);
  void (*pop)(ListObject *pobj, Var **ppv);
  void (*shift)(ListObject *pobj, Var **ppv);
  void (*unshift)(ListObject *pobj, Var *pv);
  int  (*isEmpty)(ListObject *pobj);
  void (*setBegin)(ListObject *pobj);
  int  (*hasCurrent)(ListObject *pobj);
  void (*setNext)(ListObject *pobj);
  void (*setPrev)(ListObject *pobj);
  void (*getCurrent)(ListObject *pobj, Var **ppv);
  void (*insertNext)(ListObject *pobj, Var *pv);
  void (*updateCurrent)(ListObject *pobj, Var *pv);
  void (*deleteCurrent)(ListObject *pobj);
};
VAR(ListObject) *List_new();

#ifdef	__cplusplus
}
#endif

#endif	/* LIST_H */

