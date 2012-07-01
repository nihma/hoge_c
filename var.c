/* 
 * File:   var.c
 * Author: Makoto Nihei
 *
 * Created on 2012/07/01, 16:11
 */

#include "var.h"
#include <stdio.h>
#include <stdlib.h>

struct var_private {
  unsigned int count;
  void         *ptr;
  void         (*destructor)(void *ptr);
};

static Var  clone(Var v);
static void end(Var* pv);
static void *ptr_get(Var v);

Var Var_new(void *ptr, void (*destructor)(void *ptr))
{
  if (ptr == NULL) return NULL;

  Var v = malloc(sizeof(struct var));
  if (v == NULL) return NULL;
  
  v->vp = malloc(sizeof(struct var_private));
  if (v->vp == NULL) {
    free(v);
    return NULL;
  }
  
  v->vp->count      = 1;
  v->vp->ptr        = ptr;
  v->vp->destructor = destructor;
  
  v->clone = clone;
  v->end   = end;
  v->ptr   = ptr_get;

  return v;
}

static Var clone(Var v)
{
  v->vp->count++;
  return v;
}

static void end(Var *pv)
{
  if (*pv == NULL) return;

  (*pv)->vp->count--;
  if ((*pv)->vp->count > 0) return;

  (*pv)->vp->destructor((*pv)->vp->ptr);
  free((*pv)->vp);
  free(*pv);
  *pv = NULL;
}

static void* ptr_get(Var v)
{
  return v->vp->ptr;
}

