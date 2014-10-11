#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <fcntl.h>

int main(int argn, char *argv[])
{
  char      *fn_audio;
  double     gain;
  int        offset;
  FILE      *fo_audio;
  char       BUFFER[2];
  int        size_file,sum;
  short int  value;
  short int *data;
  int        i; 
  /*Verificar Argumentos*/
  if(argn < 4){
    printf("Argumentos insuficientes. .. \nExit... \n");
    return 1;
    }
  fn_audio = argv[1];
  gain     = atof(argv[2]);
  offset   = atoi(argv[3]);  
  if (gain < 1){
    printf("Ganancia debe ser mayor a uno \n");
    return 1; 
    }
  /*Abrir archivo de audio*/
  int filedesc = open(fn_audio, O_RDONLY);
  if(filedesc < 0)
    return 1;
  size_file = 0;
  while(read(filedesc, BUFFER , 2) != 0) 
    size_file++;
  printf("size: %d\n",size_file);
  data = calloc(size_file,sizeof(short int));
  close(filedesc);
  filedesc = open(fn_audio, O_RDONLY);
  if(filedesc < 0)
    return 1;
  for(i = 0; i<size_file;i++){
    sum = read(filedesc, BUFFER , 2);
    data[i] = (short int)*BUFFER;
    //printf("NUMERO: %d, %d\n",data[i],sum);
    }

  /* Ver si fue ingresado el parametro [p] 
   *Se deben reproducir los tres archivos de audio*/
  if (argn == 5){
    printf("Parametro [p] recivido \n");
    return 0;
    }
  
  return 0;
}
