elo330_tarea2_altamirano_marquez
================================

Tarea 2 

Los archivos necesarios para compilar el programa son:

  csa.c
  csa.h
  
Se incluye un makefile en donde se especifica el comando para compilar por medio de gcc

El sistema necesita tener instalado las siguientes aplicaciones para poder ejecutar el programa:

  octave
  aplay
  
Para ejecutar el programa, es necesario ubicar en la misma carpeta:

  archivo de sonido *.raw 16 bits LE, mono
  octave_notebook.m
  sound_plots.m
  
Para ejecutar, se deben incluir los siguientes parámetros

  ./csa "sonido.raw" [gain] [offset] [p]
  
Si se ejecuta a través de SSH, es necesario activar X11 forwarding.
  
  ssh -X usuario@host
