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

static Var  *clone(Var *pv);
static void end(Var **ppv);
static void *ptr_get(Var *pv);

Var *Var_new(void *ptr, void (*destructor)(void *ptr))
{
  if (ptr == NULL) return NULL;

  Var *pv = malloc(sizeof(struct var));
  if (pv == NULL) return NULL;
  
  pv->vp = malloc(sizeof(struct var_private));
  if (pv->vp == NULL) {
    free(pv);
    return NULL;
  }
  
  pv->vp->count      = 1;
  pv->vp->ptr        = ptr;
  pv->vp->destructor = destructor;
  
  pv->clone = clone;
  pv->end   = end;
  pv->ptr   = ptr_get;

  return pv;
}

static Var *clone(Var *pv)
{
  pv->vp->count++;
  return pv;
}

static void end(Var **ppv)
{
  if (*ppv == NULL) return;

  (*ppv)->vp->count--;
  if ((*ppv)->vp->count > 0) return;

  (*ppv)->vp->destructor((*ppv)->vp->ptr);
  free((*ppv)->vp);
  free(*ppv);
  *ppv = NULL;
}

static void* ptr_get(Var *pv)
{
  return pv->vp->ptr;
}

