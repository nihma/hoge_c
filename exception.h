/* 
 * File:   exception.h
 * Author: Makoto Nihei
 *
 * Created on 2012/06/29, 23:07
 */

#ifndef EXCEPTION_H
#define	EXCEPTION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <setjmp.h>
#include "var.h"

typedef struct exception Exception;
struct exception {
  VAR(char) *message;
  int       code;
};
DECLARE_VAR(Exception);

jmp_buf *exception_begin();
void exception_end();
void exception_throw(VAR(Exception) *pe);
void exception_catch(VAR(Exception) **ppe);
VAR(Exception) *expeption_new(char *message);
const char *exception_message(VAR(Exception) *pe);

#define TRY_BEGIN      { \
                         if ( setjmp(*(exception_begin())) == 0) {
#define CATCH(_V)        } else { \
                           VAR(Exception) *_V; \
                           exception_catch(&_V);
#define FINALLY          } \
                         {
#define TRY_END            exception_end(); \
                         } \
                       }
#define THROW(_V) { \
                    exception_throw(_V); \
                  }

#ifdef	__cplusplus
}
#endif

#endif	/* EXCEPTION_H */

