#include "csa.h"

int main(int argn, char *argv[])
{
  char      *fn_audio;              //nombre del archivo a procesar
  double     gain;                  //ganancia aplicada
  int        offset;                //offset de tiempo en [ms]
  short int *raw_data;              //archivo original
  short int *sat_data;              //archivo saturado
  short int *fix_data;              //archivo con el fit realizado
  int        i,j; 
  int       *size_data;             //puntero hacia el numero de muestras del archivo de audio
  pid_t      pid;
  int        pipe1[2];              //pipe donde el proceso padre escribe
  int        pipe2[2];              //pipe donde el proceso hijo (octave) escribe
  char       str [100];
  char       readbuffer[100]; 
  
  size_data = (int*)malloc(sizeof(int));  
  
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

  /*Saturacion de datos:aplica la ganancia al archivo y lo reescala a la vez*/
  sat_data = saturar(raw_data,*size_data,gain);

  /*Crear pipes*/
  if (pipe(pipe1) < 0) {
    perror("pipe1");
    exit(1);
  }
  if (pipe(pipe2) < 0) {
    perror("pipe2");
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
  /*Configurar pipe*/
  close(pipe1[0]);
  close(pipe2[1]);

  /*crea archivo*/
  FILE* sd = fdopen(pipe1[1],"w");
  FILE* sd2 = fdopen(pipe2[0],"r");
  
  /*Enviar datos a octave*/
  fprintf(sd,"offset=%d\n",offset); 
  fflush(sd);
  
  /*Enviar Raw data*/
  fprintf(sd,"raw=[");
  for (i=0; i < *size_data ; i++){
    fprintf(sd,"%hd ",raw_data[i]);
    }
  fprintf(sd," ];\n"); 
  fflush(sd);

  /*Enviar Sat_data*/
  fprintf(sd,"sat=[");
  for (i=0; i < *size_data ; i++){
    fprintf(sd,"%hd ",sat_data[i]);
    }
  fprintf(sd," ];\n"); 
  fflush(sd);
  printf("\nDatos enviados \n");
   
  /*Enviar comandos para graficar*/ 
  fprintf(sd,"octave_notebook\n"); 
  fflush(sd);

  fprintf(sd,"sound_plots\n"); 
  fflush(sd);
  printf("Imprimiendo gráficos \n");
  
  /*--Recibir Fix data--*/
  printf("Recibiendo señal suavizada \n");
  get_fix_data(&fix_data,size_data, &sd, &sd2);
  /*-- Fix data complete--*/
  
  /*Calculo error*/
  float error;
  error=0;
  for (i=0;i<*size_data;i++){
    error = error + (fix_data[i]-raw_data[i])*(fix_data[i]-raw_data[i]);
    }
  printf("\nERROR: %f \n",error);
  error = sqrt(error/ *size_data);
  printf("DESVIACIÓN ESTÁNDAR: %f \n\n",error);
  
  /*Guardar datos*/
  escritura_audio(fix_data, (char*)"fix_data.raw" ,*size_data);
  escritura_audio(sat_data, (char*)"sat_data.raw" ,*size_data);

  
  /*Reproducir archivos*/
  if ( argn==5 && strcmp(argv[4],"p") == 0  ){
    sprintf(str,"aplay --format=S16_LE -t raw %s", fn_audio);
    system(str);
    system("aplay --format=S16_LE -t raw sat_data.raw");
    system("aplay --format=S16_LE -t raw fix_data.raw");
  }

  printf("Presione Enter para cerrar\n");
  getchar();
  fclose(sd);
  fclose(sd2);
  return 0;
}
/*-------------------------*/

/* Implementacion funciones  */
short int * lectura_audio(char *fn_audio,int *size_data)
{
  FILE *filedesc;
  int        size_file,i,sum;
  short int *raw_data;
  short int BUFFER[1];

  filedesc = fopen(fn_audio, "r");
  size_file = 0;

  /*encuentra el tamaño del archivo*/
  while(fread(BUFFER, 2 , 1,filedesc) != 0)
    size_file++;

  rewind(filedesc);
  raw_data = (short int*)calloc(size_file,sizeof(short int));
  fread(raw_data, 2 , size_file,filedesc);
  fclose(filedesc);

  *size_data = size_file;
  return raw_data;
}


short int * saturar(short int * raw_data, int size_data, double gain)
{
  short int * sat_data;
  int         i; 
  char       BUFFER[2]; 
  
  sat_data = (short int*)calloc(size_data,sizeof(short int));
  for(i = 0; i<size_data;i++){
    if ( gain*raw_data[i] >= 32767 )
    {
      sat_data[i] = 32767/gain;
    }
    else if ( gain*raw_data[i] <= -32767 )
    {
      sat_data[i] = -32767/gain;   
    } 
    else
      sat_data[i] = raw_data[i];
    }
  return sat_data;
}

void get_fix_data(short int ** fix_file,int* size_data, FILE** pd, FILE** pd2)
{
  char       str[1000];
  char       readbuffer[1000]; 
  int        i; 
  short int *fix_data;
  FILE* sd;
  FILE* sd2;
  sd =*pd;
  sd2 =*pd2;
  
  *fix_file = (short int*)calloc(*size_data,sizeof(short int));
  fix_data = *fix_file;
  fprintf(sd,"format int_16\n"); 
  fflush(sd);
  fprintf(sd,"disp(x(1:10)) \n");
  fflush(sd);
  fscanf(sd2," %[^\n]",readbuffer);
  fscanf(sd2," %[^\n]",readbuffer);
  
  /*Hay que quitar los caracteres no deseados*/
  sscanf(readbuffer,"octave:2> octave:3> octave:4> octave:4> octave:5> octave:6>  %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd ",fix_data);  
  for (i=10; i<*size_data-10 ;i=i+10){
    fprintf(sd,"disp(raw(%d+1:%d+10)) \n",i,i); 
    fflush(sd);
    fscanf(sd2, " %[^\n]",readbuffer);
    sscanf(readbuffer,"%s %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd",str,&fix_data[i],&fix_data[i+1],&fix_data[i+2],&fix_data[i+3],&fix_data[i+4],&fix_data[i+5],&fix_data[i+6],&fix_data[i+7],&fix_data[i+8],&fix_data[i+9]);
    }

  fprintf(sd,"disp(raw(%d))\n",*size_data);
  fflush(sd);
  fscanf(sd2, " %[^\n]",readbuffer);
  sscanf(readbuffer,"%s %hd",str ,&fix_data[*size_data-1]);
}

void escritura_audio(short int * audio_data, char* name ,int size_data){
  FILE * audio_file = fopen(name, "wb");
  fwrite(audio_data, 2, size_data, audio_file);
}
