/* 
 * File:   main.c
 * Author: Makoto Nihei
 *
 * Created on 2012/06/29, 6:31
 */

#include <stdio.h>
#include <stdlib.h>
#include "exception.h"

void sample_exception();

/*
 * 
 */
int main(int argc, char** argv)
{
  printf("Hello World!\n");

  sample_exception();

  return (EXIT_SUCCESS);
}

void exception_sub1();
void exception_sub2();

void sample_exception()
{
  TRY_BEGIN {
    exception_sub1();
  }
  CATCH(int, a) {
    printf("catch1 %d\n", *a);
    free(a);
  }
  FINALLY {
    printf("finally1\n");       /* 表示される */
  }
  TRY_END
}

/* 例外を投げる関数 */
void exception_sub1()
{
  printf("sub1 start\n");      /* 表示される */

  TRY_BEGIN {  /* 例外の入れ子 */
    exception_sub2();
  }
  CATCH(int, a) {
    printf("catch2 %d\n", *a);
    free(a);
  }
  FINALLY {
    printf("finally2\n");       /* 表示される */
  }
  TRY_END

  {
    int *t = malloc(sizeof(int));
    if (t == NULL) exit(1);
    *t = 100;
    THROW(t);
  }
  printf("sub1 end\n");        /* 表示されない */
}

/* 例外を投げる関数 */
void exception_sub2()
{
  printf("sub2 start\n");      /* 表示される */
  {
    int *t = malloc(sizeof(int));
    if (t == NULL) exit(1);
    *t = 200;
    THROW(t);
  }
  printf("sub2 end\n");        /* 表示されない */
}
