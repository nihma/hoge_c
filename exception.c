/* 
 * File:   exception.c
 * Author: Makoto Nihei
 *
 * Created on 2012/06/29, 23:07
 */

#include "exception.h"
#include <stdlib.h>

typedef struct EXCEPTION {
  jmp_buf buf;
  void* value;
} EXCEPTION;

typedef struct EXCEPTION_STACK EXCEPTION_STACK;
struct EXCEPTION_STACK {
  EXCEPTION_STACK *prev;
  EXCEPTION       *e;
};

static EXCEPTION       e_first;  // メモリエラー対策
static EXCEPTION_STACK e_stack = {NULL, &e_first};
static int             e_num   = 0;  // 例外要素数

jmp_buf *exception_begin()
{
  e_num++;
  if (e_num == 1) return &(e_stack.e->buf);  // 1回目の呼び出しは静的確保済み
  
  EXCEPTION_STACK *e_stack_prev = malloc(sizeof(EXCEPTION_STACK));
  if (e_stack_prev == NULL) {
    e_num--;  // 今回のスタートは無かったことにする
    exception_throw(NULL);  // TODO: メモリエラー例外
  }
    
  e_stack_prev->e    = e_stack.e;
  e_stack_prev->prev = e_stack.prev;

  e_stack.e    = malloc(sizeof(EXCEPTION));
  if (e_stack.e == NULL) {
    e_stack.e = e_stack_prev->e;
    free(e_stack_prev);
    e_num--;  // 今回のスタートは無かったことにする
    exception_throw(NULL);  // TODO: メモリエラー例外
  }
  e_stack.prev = e_stack_prev;

  return &(e_stack.e->buf);
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

void exception_throw(void *value)
{
  if (e_num == 0) return;

  e_stack.e->value = value;
  longjmp(e_stack.e->buf, 1);
}

void exception_catch(void **value)
{
  if (e_num == 0) return;

  *value = e_stack.e->value;
}
