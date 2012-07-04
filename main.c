/* 
 * File:   main.c
 * Author: Makoto Nihei
 *
 * Created on 2012/06/29, 6:31
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"
#include "var.h"

void sample_exception();
void sample_var();

/*
 * 
 */
int main(int argc, char** argv)
{
  printf("Hello World!\n");

  sample_exception();
  sample_var();

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

void sample_var()
{
  VAR(int) aa =  VAR_NEW_SIMPLE(int)();
  if (aa == NULL) exit(-1);

  VAR(int) bb = aa->clone(aa);
  *(VAR_PTR(aa)) = 105;

  aa->end(aa);  /* まだ開放されない */

  printf("%d\n", *(VAR_PTR(bb)));
  bb->end(bb); /* ここで開放 */

  VAR(char) cc = VAR_NEW_ARRAY_SIMPLE(char)(10);
  if (cc == NULL) exit(-1);
  
  strncpy(VAR_PTR(cc), "hogehoge", 8);
  printf("%s\n", VAR_PTR(cc));
  cc->end(cc);
}
