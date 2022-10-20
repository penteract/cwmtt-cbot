// Rough Memory bound: 100MiB
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <board.c>
#include <pgnparser.c>
#include <printing.c>

char input[10000];

int main(int argc, char** argv){
  char* t = &(BRD(0,MAXL,0,0,0));
  *(t+1) = 'B';
  setup();
  printBoard();

  assert(argc>1);
  printf("%s\n",argv[1]);
  FILE* f=fopen(argv[1],"r");
  printf("--- %ld ---\n",(long int)f);
  if(f==0) return 1;
  int n = fread(input,1,9999,f);
  input[n]='\0';
  printf("bytes read %d\n", n);
  printf("parse result %d\n",parse(input));
  printf("minL %d  maxL %d\n", minL,maxL);
  printBoard();
  //printf("%c\n",BRD(0,MAXL,0,0,1));
  //printf("%d\n",(int)((unsigned char)('A'|0x80)));
  printf("%x\n", (unsigned long)board );
  return 0;
}
