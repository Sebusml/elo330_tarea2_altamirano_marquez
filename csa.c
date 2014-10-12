#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SLEEP_T 0.5

short int * lectura_audio(char *,int *);
short int * saturar(short int *,int,double);
void   w_octave(char* , int );
void   r_octave(char* );

int main(int argn, char *argv[])
{
  char      *fn_audio;
  double     gain;
  int        offset;
  short int *raw_data;
  short int *sat_data;
  short int *fix_data;
  int        i,status,nbytes; 
  int       *size_data;
  pid_t      pid;
  int        pipe1[2];    //pipe que recibe desde octave
  int        pipe2[2];    /*pipe que escribe en octave *pipe original*/
  char       str [100];
  char       readbuffer[100]; 
  size_data = malloc(sizeof(int));
  
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
  /*Lectura datos*/
  raw_data = lectura_audio(fn_audio,size_data);
  /*Saturacion de datos*/
  sat_data = saturar(raw_data,*size_data,gain);
  /*Crear pipes*/
  if (pipe(pipe1) < 0) {
    perror("input pipe");
    exit(1);
  }
  if (pipe(pipe2) < 0) {
    perror("output pipe");
    exit(1);
  }
  if ((pid = fork()) < 0) {
    perror("fork");
    exit(1);
  }
  /*Proceso Hijo */
  if (pid == 0) {                    
    int junk;
    dup2(pipe1[0], 0);                
    dup2(pipe2[1], 1);       
    close(pipe1[1]);          
    close(pipe2[0]);          
    junk = open("/dev/null", O_WRONLY);
    dup2(junk, 2);  /* throw away any error message */
    execlp("octave", "octave", "-i", "-q", 0);
    perror("exec");
    exit(-1); 
  }
  /*-------------*/
  /*Proceso Padre*/
  close(pipe1[0]);
  close(pipe2[1]);
  FILE* sd = fdopen(pipe1[1],"w");
  FILE* sd2 = fdopen(pipe2[0],"r");
  /*Enviar datos a octave*/
  /*Enviar Raw data*/
  fprintf(sd,"raw=[");
  for (i=0; i < *size_data ; i++){
    fprintf(sd,"%hd ",raw_data[i]);
    }
  fprintf(sd," ];\n"); 
  fflush(sd);
  printf("Raw_data enviada \n");
  
  //fprintf(sd,"plot(1,2)\n");fflush(sd);
  //sleep(20);
  fprintf(sd,"format free;disp(raw(1:10)) \n");
  fflush(sd);
  
  fflush(sd2);
  fscanf(sd2,"octave:1> octave:2>  %[^\n] ",readbuffer);
  
  //NO es necesario este nums, ver mas abajo
  int nums[10];
  sscanf(readbuffer,"%d %d %d %d %d %d %d %d %d",&nums[0],&nums[1],&nums[2],&nums[3],&nums[4],&nums[5],&nums[6],&nums[7],&nums[8],&nums[9]);
  
  fix_data = calloc(*size_data,sizeof(short int));
  for(i=0;i<10;i++){
    fix_data[i] = nums[i];
    //printf("%d ",fix_data[i]);  
  }
  int k; 
  int j = 3;
  for (i=10; i<*size_data-10;i=i+10){
    fprintf(sd,"disp(raw(%d+1:%d+10)) \n",i,i); 
    fflush(sd);
    
    fflush(sd2);
    fscanf(sd2, " %[^\n]",readbuffer);
    
    //sscanf(readbuffer,"%s %d %d %d %d %d %d %d %d %d %d",str,&nums[0],&nums[1],&nums[2],&nums[3],&nums[4],&nums[5],&nums[6],&nums[7],&nums[8],&nums[9]);
    sscanf(readbuffer,"%s %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd",str,&fix_data[i],&fix_data[i+1],&fix_data[i+2],&fix_data[i+3],&fix_data[i+4],&fix_data[i+5],&fix_data[i+6],&fix_data[i+7],&fix_data[i+8],&fix_data[i+9]);
    //for(k=0;k<10;k++){
    //  fix_data[i+k] = nums[k]; 
    //  }
    j++;
    }
  fprintf(sd,"disp(raw(%d))\n",*size_data);
  fflush(sd);
  fscanf(sd2, " %[^\n]",readbuffer);
  sscanf(readbuffer,"%s %hd",str ,&fix_data[*size_data-1]);
  for (i=0;i<*size_data;i++){
    printf("%d %d \n",raw_data[i],fix_data[i]);  
    }
  //faltan los ultimos
  printf("%d \n",*size_data);
  fclose(sd);
  fclose(sd2);
  return 0;
}
/*-------------------------*/




short int * lectura_audio(char *fn_audio,int *size_data)
{
  int filedesc;
  int        size_file,i,sum;
  short int *raw_data;
  char       BUFFER[2];
  filedesc = open(fn_audio, O_RDONLY);
  if(filedesc < 0)
    return NULL;
  size_file = 0;
  while(read(filedesc, BUFFER , 2) != 0) 
    size_file++;
  raw_data = calloc(size_file,sizeof(short int));
  close(filedesc);
  filedesc = open(fn_audio, O_RDONLY);
  if(filedesc < 0)
    return NULL;
  for(i = 0; i<size_file;i++){
    sum = read(filedesc, BUFFER , 2);
    raw_data[i] = (short int)*BUFFER;
    //printf("NUMERO: %d, %d\n",raw_data[i],sum);
    }
  close(filedesc);
  *size_data = size_file;
  return raw_data;
}

short int * saturar(short int * raw_data, int size_data, double gain)
{
  short int * sat_data;
  int         i; 
  char       BUFFER[2]; 

  sat_data = calloc(size_data,sizeof(short int));
  for(i = 0; i<size_data;i++){
    if ( gain*raw_data[i] >= 32767 )
      sat_data[i] = 32767/gain;
    if ( gain*raw_data[i] <= -32767 )
      sat_data[i] = -32767/gain;   
    sat_data[i] = raw_data[i];
    //printf("sat: %hd\n",sat_data[i]);
    }  
  return sat_data;
}


