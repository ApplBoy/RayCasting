#!/bin/bash
clear
if (gcc main.c -lm -ldl -lglut -lGL -lGLU -lGLEW -o OutTeste.out);then
	echo
	echo "OK"
	./OutTeste.out
else
	echo
	echo "ERRO!"
fi
