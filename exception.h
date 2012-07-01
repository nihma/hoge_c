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

jmp_buf *exception_begin();
void exception_end();
void exception_throw(void *value);
void exception_catch(void **value);

#define TRY_BEGIN      { \
                         if ( setjmp(*(exception_begin())) == 0) {
#define CATCH(_TYPE, _V) } else { \
                           _TYPE *_V; \
                           exception_catch((void**)(&_V));
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

