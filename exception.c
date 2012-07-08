/* 
 * File:   exception.c
 * Author: Makoto Nihei
 *
 * Created on 2012/06/29, 23:07
 */

#include "exception.h"
#include <stdlib.h>
#include <string.h>

typedef struct throw_data {
  jmp_buf        buf;
  VAR(Exception) *pe;
} ThrowData;

typedef struct EXCEPTION_STACK EXCEPTION_STACK;
struct EXCEPTION_STACK {
  EXCEPTION_STACK *prev;
  ThrowData       *e;
};

static ThrowData       e_first;  // メモリエラー対策
static EXCEPTION_STACK e_stack = {NULL, &e_first};
static int             e_num   = 0;  // 例外要素数

jmp_buf *exception_begin()
{
  if (e_num == 0) goto END;  // 1回目の呼び出しは静的確保済み

  EXCEPTION_STACK *e_stack_prev = malloc(sizeof(EXCEPTION_STACK));
  if (e_stack_prev == NULL) goto ERROR;
    
  e_stack_prev->e    = e_stack.e;
  e_stack_prev->prev = e_stack.prev;

  e_stack.e    = malloc(sizeof(ThrowData));
  if (e_stack.e == NULL) {
    e_stack.e = e_stack_prev->e;
    free(e_stack_prev);
    goto ERROR;
  }
  e_stack.prev = e_stack_prev;

END:
  e_num++;
  return &(e_stack.e->buf);

ERROR:
  exception_throw(NULL);  // TODO: メモリエラー例外
}

void exception_end()
{
  if (e_num == 0) return;
  else if (e_num == 1) {
    e_stack.e    = &e_first;
    e_stack.prev = NULL;
  }
  else {
    free(e_stack.e);

    EXCEPTION_STACK *e_stack_prev = e_stack.prev;
    e_stack.e    = e_stack_prev->e;
    e_stack.prev = e_stack_prev->prev;
    free(e_stack_prev);
  }
  e_num--;
}

void exception_throw(VAR(Exception) *pe)
{
  if (e_num == 0) {
    /* not found try/catch */
    exit(1);
  }

  e_stack.e->pe = pe;
  longjmp(e_stack.e->buf, 1);
}

void exception_catch(VAR(Exception) **ppe)
{
  if (e_num == 0) return;

  *ppe = e_stack.e->pe;
}

static void exception_destructor(void *pv)
{
  if (pv == NULL) return;

  Exception *pe = (Exception *)pv;
  pe->message->end(pe->message);
  free(pe);
}

VAR(Exception) *expeption_new(char *message)
{
  VAR(Exception) *pv = VAR_NEW(Exception)(malloc(sizeof(Exception)),
                                          exception_destructor);
  if (pv == NULL) return pv;

  VAR_PTR(pv)->message = VAR_NEW_ARRAY_SIMPLE(char)(strlen(message) + 1);
  if (VAR_PTR(pv)->message == NULL) return pv;

  strcpy(VAR_PTR(VAR_PTR(pv)->message), message);

  return pv;
}

const char *exception_message(VAR(Exception) *pe)
{
  return VAR_PTR(VAR_PTR(pe)->message);
}
