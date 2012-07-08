/* 
 * File:   var.h
 * Author: Makoto Nihei
 *
 * Created on 2012/07/01, 16:11
 */

#ifndef VAR_H
#define	VAR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
  
/* 参照カウンタ */  

typedef struct var_private* VarPrivate;
typedef struct var * Var;
struct var {
  VarPrivate vp;
  Var        (*clone)(Var);
  void       (*end)(Var*);
  void       *(*ptr)(Var);
};
Var Var_new(void *ptr, void (*destructor)(void *ptr));

/* タイプセーフなvar */

#define VAR(TYPE) Var##TYPE
#define VAR_NEW(TYPE) Var##TYPE##_new
#define VAR_NEW_SIMPLE(TYPE) Var##TYPE##_new_simple
#define VAR_NEW_ARRAY_SIMPLE(TYPE) Var##TYPE##_new_array_simple

#define DECLARE_VAR_TYPE(TYPE) \
typedef struct var_##TYPE VAR(TYPE); \
struct var_##TYPE { \
  Var         org; \
  VAR(TYPE)   *(*clone)(VAR(TYPE) *); \
  void        (*end)(VAR(TYPE) *); \
  TYPE        *(*ptr)(VAR(TYPE) *); \
};

#define VAR_PTR(pv) pv->ptr(pv)

#define DECLARE_VAR_NEW(TYPE) \
static VAR(TYPE) *Var##TYPE##_clone(VAR(TYPE) *pv) { \
  pv->org = pv->org->clone(pv->org); \
  return pv; \
} \
static void Var##TYPE##_end(VAR(TYPE) *pv) { \
  pv->org->end(&(pv->org)); \
  if (pv->org == NULL) { \
    free(pv); \
  } \
} \
static TYPE *Var##TYPE##_ptr(VAR(TYPE) *pv) { \
  return (TYPE *) pv->org->ptr(pv->org); \
} \
static VAR(TYPE) *VAR_NEW(TYPE)(TYPE *ptr, void (*destructor)(void *ptr)) { \
  VAR(TYPE) *pv = malloc(sizeof(struct var_##TYPE)); \
  if (pv == NULL) return pv; \
  pv->org = Var_new(ptr, (void (*)(void *))destructor); \
  if (pv->org == NULL) { \
    free(pv); \
    return NULL; \
  } \
  pv->clone = Var##TYPE##_clone; \
  pv->end   = Var##TYPE##_end; \
  pv->ptr   = Var##TYPE##_ptr; \
  return pv; \
} \
static VAR(TYPE) *VAR_NEW_SIMPLE(TYPE)() { \
  return VAR_NEW(TYPE)(malloc(sizeof(TYPE)), free); \
} \
static VAR(TYPE) *VAR_NEW_ARRAY_SIMPLE(TYPE)(unsigned int num) { \
  return VAR_NEW(TYPE)(malloc(sizeof(TYPE) * num), free); \
}

#define DECLARE_VAR(TYPE) \
  DECLARE_VAR_TYPE(TYPE); \
  DECLARE_VAR_NEW(TYPE);

/* 基本データ型 */
DECLARE_VAR(long);
DECLARE_VAR(int);
DECLARE_VAR(char);
DECLARE_VAR(float);
DECLARE_VAR(double);

#ifdef	__cplusplus
}
#endif

#endif	/* VAR_H */

