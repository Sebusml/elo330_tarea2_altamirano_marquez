#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

short int * lectura_audio(char *,int *);
void        escritura_audio(short int *, char*,int);
short int * saturar(short int *,int,double);
void        get_fix_data(short int **,int*,FILE**,FILE**);