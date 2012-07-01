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

#ifdef	__cplusplus
}
#endif

#endif	/* VAR_H */

