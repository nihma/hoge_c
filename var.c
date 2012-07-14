/* 
 * File:   var.c
 * Author: Makoto Nihei
 *
 * Created on 2012/07/01, 16:11
 */

#include "var.h"
#include <stdio.h>
#include <stdlib.h>

struct var_local {
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
  if (pv == NULL) {
    destructor(ptr);
    return NULL;
  }

  pv->pl = malloc(sizeof(struct var_local));
  if (pv->pl == NULL) {
    free(pv);
    destructor(ptr);
    return NULL;
  }
  
  pv->pl->count      = 1;
  pv->pl->ptr        = ptr;
  pv->pl->destructor = destructor;
  
  pv->clone = clone;
  pv->end   = end;
  pv->ptr   = ptr_get;

  return pv;
}

static Var *clone(Var *pv)
{
  pv->pl->count++;
  return pv;
}

static void end(Var **ppv)
{
  if (*ppv == NULL) return;

  (*ppv)->pl->count--;
  if ((*ppv)->pl->count > 0) return;

  (*ppv)->pl->destructor((*ppv)->pl->ptr);
  free((*ppv)->pl);
  free(*ppv);
  *ppv = NULL;
}

static void* ptr_get(Var *pv)
{
  return pv->pl->ptr;
}

