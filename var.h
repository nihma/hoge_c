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

#define DECLARE_VAR_TYPE(TYPE) \
typedef struct var_##TYPE* VAR(TYPE); \
struct var_##TYPE { \
  Var        org; \
  VAR(TYPE)  (*clone)(VAR(TYPE)); \
  void       (*end)(VAR(TYPE)); \
  TYPE *     (*ptr)(VAR(TYPE)); \
};

#define VAR_PTR(v) v->ptr(v)

#define DECLARE_VAR_NEW(TYPE) \
static VAR(TYPE) Var##TYPE##_clone(VAR(TYPE) v) { \
  v->org = v->org->clone(v->org); \
  return v; \
} \
static void Var##TYPE##_end(VAR(TYPE) v) { \
  v->org->end(&(v->org)); \
  if (v->org == NULL) { \
    free(v); \
  } \
} \
static TYPE *Var##TYPE##_ptr(VAR(TYPE) v) { \
  return (TYPE *) v->org->ptr(v->org); \
} \
static VAR(TYPE) VAR_NEW(TYPE)(TYPE *ptr, void (*destructor)(void *ptr)) { \
  VAR(TYPE) v = malloc(sizeof(struct var_##TYPE)); \
  if (v == NULL) return v; \
  v->org = Var_new(ptr, (void (*)(void *))destructor); \
  if (v->org == NULL) { \
    free(v); \
    return NULL; \
  } \
  v->clone = Var##TYPE##_clone; \
  v->end   = Var##TYPE##_end; \
  v->ptr   = Var##TYPE##_ptr; \
  return v; \
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

