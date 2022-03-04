// Copyright Spinochi Andreea -312CA

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "definitions.h"

void load_file(char **p, FILE * *ptr, image * *eptr, colour * **matrix, int *ok,
			   int *x1, int *y1, int *x2, int *y2);
void alloc_mem(colour ***matrix, double lines, double colums, int *ok);
void free_mem(colour ***matrix, double lines, int *ok);
void skiplines(FILE **ptr);
void load_ASCII(colour ***matrix, image **eptr, FILE **ptr);
void load_BINARY(colour ***matrix, image **eptr, FILE **ptr, char **p);
void SELECT__ALL(image **eptr, int *x1, int *y1, int *x2, int *y2);
void SELECT(int *x1, int *y1, int *x2, int *y2,  image **eptr, FILE **ptr,
			int a, int b, int c, int d);
void ROTATE(colour ***matrix, image **eptr, int angle, int *x1, int *y1,
			int *x2, int *y2, int *ok, FILE **ptr);
void swap(image **eptr);
void ROTATE_angle(int angle, int submatrix, int *x1, int *y1, int *x2, int *y2,
				  int *ok, colour ***matrix, image **eptr);
void CROP_image(colour ***matrix, int *x1, int *y1, int *x2, int *y2, int *ok,
				image **eptr, FILE **ptr);
double clamp(double x, double min, double max);
void EDGE(colour **mat, colour ***matrix, int i, int j);
void SHARPEN(colour **mat, colour ***matrix, int i, int j);
void BLUR(colour **mat, colour ***matrix, int i, int j);
void GAUSSIAN_BLUR(colour **mat, colour ***matrix, int i, int j);
void APPLY(colour ***matrix, int *x1, int *y1, int *x2, int *y2, image **eptr,
		   char **p, int *ok);
void SAVE_ascii(colour ***matrix, char *save, image **eptr, FILE **ptr);
void SAVE_binary(colour ***matrix, char *save, image **eptr, FILE **ptr);
void EXIT(FILE **ptr, colour ***matrix, int *ok, image **eptr);
int is_number(char *str);
int invalid_select(char **p, int *a, int *b, int *c, int *d);
void read_command(char **command);
void check_selection(char **p, image **eptr, int *x1, int *y1, int *x2,
					 int *y2, FILE **ptr);
void check_apply(char **p, int *ok, image **eptr, colour ***matrix, int *x1,
				 int *y1, int *x2, int *y2);

#endif
