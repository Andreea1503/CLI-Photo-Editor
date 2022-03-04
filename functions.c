// Copyright Spinochi Andreea -312CA

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "functions.h"
#include "definitions.h"

// function to read the command
void read_command(char **command)
{
	size_t len = 4000;

	// allocating memory
	(*command) = (char *)malloc(len);

	// defensive programming
	if ((*command) == NULL) {
		fprintf(stderr, "Malloc failed\n");
		(*command) = NULL;
	}

	// reading the command
	getline(&(*command), &len, stdin);
}

// function to check if the selection is valid and selecting the coordinates
// if it is
void check_selection(char **p, image **eptr, int *x1, int *y1, int *x2,
					 int *y2, FILE **ptr)
{
	(*p) = strtok(NULL, " ");

	if (strcmp((*p), "ALL") == 0) {
		if ((*eptr)->height == -1 || (*eptr)->width == -1 ||
			(*ptr) == 0) {
			printf("No image loaded\n");

		} else {
			printf("Selected ALL\n");

			SELECT__ALL(&(*eptr), &(*x1), &(*y1), &(*x2), &(*y2));
		}
	} else {
		int a, b, c, d;

		if (invalid_select(&(*p), &a, &b, &c, &d) == 1)
			SELECT(&(*x1), &(*y1), &(*x2), &(*y2), &(*eptr), &(*ptr),
				   a, b, c, d);
	}
}

// function to check if the apply command is valid and applying the filters
// if it is
void check_apply(char **p, int *ok, image **eptr, colour ***matrix, int *x1,
				 int *y1, int *x2, int *y2)
{
	(*p) = strtok(NULL, " ");

	if ((*ok) == 1) {
		if (!(*p)) {
			printf("Invalid command\n");

		} else {
			if (strcmp((*eptr)->magic_word, "P3") == 0 ||
				strcmp((*eptr)->magic_word, "P6") == 0) {
				if (strcmp((*p), "EDGE") == 0) {
					printf("APPLY EDGE done\n");

					APPLY(&(*matrix), &(*x1), &(*y1), &(*x2), &(*y2), &(*eptr),
						  &(*p), &(*ok));

				} else if (strcmp((*p), "SHARPEN") == 0) {
					printf("APPLY SHARPEN done\n");

					APPLY(&(*matrix), &(*x1), &(*y1), &(*x2), &(*y2), &(*eptr),
						  &(*p), &(*ok));

				} else if (strcmp((*p), "BLUR") == 0) {
					printf("APPLY BLUR done\n");

					APPLY(&(*matrix), &(*x1), &(*y1), &(*x2), &(*y2), &(*eptr),
						  &(*p), &(*ok));

				} else if (strcmp((*p), "GAUSSIAN_BLUR") == 0) {
					printf("APPLY GAUSSIAN_BLUR done\n");

					APPLY(&(*matrix), &(*x1), &(*y1), &(*x2), &(*y2), &(*eptr),
						  &(*p), &(*ok));

				} else {
					printf("APPLY parameter invalid\n");
					}
				}
			if (strcmp((*eptr)->magic_word, "P2") == 0 ||
				strcmp((*eptr)->magic_word, "P5") == 0)
				printf("Easy, Charlie Chaplin\n");
			}
	} else {
		printf("No image loaded\n");
	}
}

// void function that loads the file
void load_file(char **p, FILE **ptr, image **eptr, colour ***matrix, int *ok,
			   int *x1, int *y1, int *x2, int *y2)
{
	// in case we load more than one file, we have to dealloc the memory of the
	// first image so we can allocate memory for a new one
	if ((*ok) == 1) {
		free_mem(&(*matrix), (*eptr)->height, &(*ok));

		// closing the first file, so we can open a new one
		fclose(*ptr);
	}

	// opening the file
	(*ptr) = fopen((*p), "r");

	// defensive programming
	if ((*ptr) == NULL) {
		printf("Failed to load %s\n", (*p));
		(*ptr) = 0;
	} else {
		printf("Loaded %s\n", (*p));

		// reading the type of the file
		fscanf((*ptr), "%s", (*eptr)->magic_word);

		// skipping commented lines
		skiplines(&(*ptr));

		// reading the width and height of the image
		fscanf((*ptr), "%lf %lf", &(*eptr)->width, &(*eptr)->height);

		// skipping commented lines
		skiplines(&(*ptr));

		// reading the maximum value for the bytes
		fscanf((*ptr), "%lf", &(*eptr)->max_value);

		// skipping commented lines
		skiplines(&(*ptr));

		// selecting the coordinates for the edges of the image
		SELECT__ALL(&(*eptr), &(*x1), &(*y1), &(*x2), &(*y2));

		// allocating memory for the image
		if (*ok == 0)
			alloc_mem(&(*matrix), (*eptr)->height, (*eptr)->width, &(*ok));

		if (strcmp((*eptr)->magic_word, "P2") == 0 ||
			strcmp((*eptr)->magic_word, "P3") == 0)
			load_ASCII(&(*matrix), &(*eptr), &(*ptr));

		if (strcmp((*eptr)->magic_word, "P5") == 0 ||
			strcmp((*eptr)->magic_word, "P6") == 0)
			load_BINARY(&(*matrix), &(*eptr), &(*ptr), &(*p));
	}
}

// function that allocates memory for the image
void alloc_mem(colour ***matrix, double lines, double colums, int *ok)
{
	(*matrix) = malloc(lines * sizeof(colour *));

	// defensive programming
	if ((*matrix) == NULL) {
		fprintf(stderr, "Malloc failed\n");
		(*matrix) = NULL;
		return;
	}

	for (int i = 0; i < lines; i++) {
		(*matrix)[i] = malloc(colums * sizeof(colour));

		// defensive programming
		if (!(*matrix)[i]) {
			fprintf(stderr, "Malloc failed\n");
			for (int j = 0; j <= i; j++)
				free((*matrix)[j]);
			free((*matrix));
			return;
		}
	}

	*ok = 1;
}

// function that deallocates the memory for the image
void free_mem(colour ***matrix, double lines, int *ok)
{
	for (int i = 0; i < lines; i++)
		free((*matrix)[i]);

	free((*matrix));

	*ok = 0;
}

// function that skips the commented lines in a file
void skiplines(FILE **ptr)
{
	// reading the first character of the line
	fgetc((*ptr));

	// if that character is hash, we skip the line
	if (fgetc((*ptr)) == '#') {
		char *buffer;
		size_t bufsize = 100;

		// allocating memory for the commented line
		buffer = (char *)malloc(bufsize * sizeof(char));

		// defensive programming
		if (!buffer) {
			fprintf(stderr, "Malloc failed\n");
			buffer = NULL;
			return;
		}

		getline(&buffer, &bufsize, (*ptr));

		// deallocating memory for the commented line
		free(buffer);

	} else {
		// if the character isn't hash, we go back to the beginning of the line
		fseek((*ptr), -1, SEEK_CUR);
	}
}

// function that load the image given in an ASCII format
void load_ASCII(colour ***matrix, image **eptr, FILE **ptr)
{
	// checking the magic number to see if the image is colored or uncolored
	if (!strcmp((*eptr)->magic_word, "P2"))
		(*eptr)->color = uncolored;
	else if (!strcmp((*eptr)->magic_word, "P3"))
		(*eptr)->color = colored;

	// if the image is uncolored, we read the matrix in grayscale format
	if ((*eptr)->color == uncolored) {
		for (int i = 0; i < (*eptr)->height; i++)
			for (int k = 0; k < (*eptr)->width; k++) {
				// reading the value of the matrix as an unsigned char
				unsigned char grey;
				fscanf((*ptr), "%hhu", &grey);

				// casting the unsigned char value to int, so we can turn it
				// into double
				int grey2 = (int)grey;

				// casting the int value into double and putting it into
				// the matrix
				(*matrix)[i][k].grey = (double)grey2;
			}

	// if the image is colored, we read the matrix in rgb format
	} else if ((*eptr)->color == colored) {
		for (int i = 0; i < (*eptr)->height; i++)
			for (int k = 0; k < (*eptr)->width; k++) {
				// reading the values of the matrix as unsigned char
				unsigned char red, green, blue;
				fscanf((*ptr), "%hhu %hhu %hhu", &red, &green, &blue);

				// casting the unsigned char values to int, so we can turn it
				// into double
				int red2 = (int)red;
				int green2 = (int)green;
				int blue2 = (int)blue;

				// casting the int values into double and putting them into
				// the matrix
				(*matrix)[i][k].red = (double)red2;
				(*matrix)[i][k].green = (double)green2;
				(*matrix)[i][k].blue = (double)blue2;
			}
	}
}

// function that load the image given in a BINARY format
void load_BINARY(colour ***matrix, image **eptr, FILE **ptr, char **p)
{
	// saving the position of the cursor in the file
	long position = ftell(*ptr);

	// closing the file
	fclose((*ptr));

	// opening the binary file
	(*ptr) = fopen((*p), "rb");

	// defensive programming
	if ((*ptr) == NULL) {
		printf("Failed to load %s\n", (*p));
		(*ptr) = 0;
		return;
	}

	// returning to the saved position of the cursor
	fseek(*ptr, position, SEEK_SET);

	// checking to see if the image is colored or uncolored
	if (!strcmp((*eptr)->magic_word, "P5"))
		(*eptr)->color = uncolored;
	else if (!strcmp((*eptr)->magic_word, "P6"))
		(*eptr)->color = colored;

	// if the image is uncolored, we read the matrix in grayscale format
	if ((*eptr)->color == uncolored) {
		for (int i = 0; i < (*eptr)->height; i++)
			for (int k = 0; k < (*eptr)->width; k++) {
				// reading the value of the matrix as an unsigned char
				unsigned char grey;
				fread(&grey, 1, 1, (*ptr));

				// casting the unsigned char value to int, so we can turn it
				// into double
				int grey2 = (int)grey;

				// casting the int value into double and putting it into
				// the matrix
				(*matrix)[i][k].grey = (double)grey2;
			}

	// if the image is colored, we read the matrix in rgb format
	} else if ((*eptr)->color == colored) {
		for (int i = 0; i < (*eptr)->height; i++)
			for (int k = 0; k < (*eptr)->width; k++) {
				// reading the value of the matrix as unsigned char
				unsigned char red, green, blue;
				fread(&red, 1, 1, (*ptr));

				// casting the unsigned char value to int, so we can turn it
				// into double
				int red2 = (int)red;

				// casting the int value into double and putting it into the
				// matrix
				(*matrix)[i][k].red = (double)red2;

				fread(&green, 1, 1, (*ptr));

				int green2 = (int)green;

				(*matrix)[i][k].green = (double)green2;

				fread(&blue, 1, 1, (*ptr));

				int blue2 = (int)blue;

				(*matrix)[i][k].blue = (double)blue2;
			}
	}
}

// function to select the coordinates for the edges of the image
void SELECT__ALL(image **eptr, int *x1, int *y1, int *x2, int *y2)
{
	(*y1) = 0;
	(*x1) = 0;
	(*y2) = (*eptr)->height;
	(*x2) = (*eptr)->width;
}

// function to make a selection of the coordinates of the matrix that
// does not exceed the bounds of the image
void SELECT(int *x1, int *y1, int *x2, int *y2,  image **eptr,
			FILE **ptr, int a, int b, int c, int d)
{
	if (a > c) {
		int aux = a;
		a = c;
		c = aux;
	}

	if (b > d) {
		int aux = b;
		b = d;
		d = aux;
	}

	if ((*eptr)->height == -1 || (*eptr)->width  == -1 || (*ptr) == 0) {
		printf("No image loaded\n");
	} else {
		if (a >= 0 && b >= 0 && c <= (*eptr)->width && d <= (*eptr)->height) {
			if (a == b && d == a) {
				printf("Invalid set of coordinates\n");
			} else if (b == c && c == d) {
				printf("Invalid set of coordinates\n");
			} else if (c == a || b == d || c <= 0 || d <= 0) {
				printf("Invalid set of coordinates\n");
			} else {
				(*x1) = a;
				(*y1) = b;
				(*x2) = c;
				(*y2) = d;
				printf("Selected %d %d %d %d\n", (*x1), (*y1), (*x2), (*y2));
			}
		} else {
			printf("Invalid set of coordinates\n");
			return;
		}
	}
}

// function that checks the angle of the rotation of the image
// or a portion of the image
void ROTATE(colour ***matrix, image **eptr, int angle, int *x1, int *y1,
			int *x2, int *y2, int *ok, FILE **ptr)
{
	// checking if the matrix that will be rotated is the full
	// image or just a portion
	int submatrix = 0;
	if ((*eptr)->height != (*y2) || (*eptr)->width != (*x2) ||
		(*x1) != 0 || (*y1) != 0)
		submatrix = 1;

	if ((*ok) == 0 || (*ptr) == 0) {
		printf("No image loaded\n");
		return;
	} else if (((*x2) - (*x1) != (*y2) - (*y1)) && submatrix == 1) {
		printf("The selection must be square\n");
	} else if (angle == 360 || angle == -360 || angle == 0) {
		printf("Rotated %d\n", angle);
	} else if (angle == 90) {
		printf("Rotated %d\n", angle);
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
	} else if (angle == -90) {
		printf("Rotated %d\n", angle);
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
	} else if (angle == 180) {
		printf("Rotated %d\n", angle);
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
	} else if (angle == -180) {
		printf("Rotated %d\n", angle);
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
	} else if (angle == 270) {
		printf("Rotated %d\n", angle);
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
	} else if (angle == -270) {
		printf("Rotated %d\n", angle);
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
		ROTATE_angle(angle, submatrix, &(*x1), &(*y1), &(*x2), &(*y2), &(*ok),
					 &(*matrix), &(*eptr));
	} else {
		printf("Unsupported rotation angle\n");
	}
}

void swap(image **eptr)
{
	double aux;
	aux = (*eptr)->height;
	(*eptr)->height = (*eptr)->width;
	(*eptr)->width = aux;
}

// function that rotates the image or the portion of the image
void ROTATE_angle(int angle, int submatrix, int *x1, int *y1, int *x2,
				  int *y2, int *ok, colour ***matrix, image **eptr)
{
	int a = (*x1), b = (*y1), c = (*x2), d = (*y2);
	colour **mat;

	// allocating memory for the auxiliary matrix
	alloc_mem(&mat, (*eptr)->height, (*eptr)->width, &(*ok));

	// copying the image in the auxiliary matrix
	for (int i = 0; i < (*eptr)->height; ++i)
		for (int j = 0; j < (*eptr)->width; ++j)
			mat[i][j] = (*matrix)[i][j];

	int left = 0, right = 0;

	// checking to see if we will rotate the matrix to the left or to the right
	if (angle < 0)
		left = 1;
	else
		right = 1;

	// if the matrix is the full image and it's a rectangular one, we deallocate
	// the memory and we alocate it again with the lines and colums switched
	if (!((*x2) - (*x1) == (*y2) - (*y1)) && submatrix == 0) {
		if ((*matrix) != NULL)
			free_mem(&(*matrix), (*eptr)->height, &(*ok));

		alloc_mem(&(*matrix), (*eptr)->width, (*eptr)->height, &(*ok));

		// switching the lines and colums
		swap(&(*eptr));
	}

	if (right == 1) {
		int l = (*y1), c = (*x1);
		for (int i = (*x1); i < (*x2); ++i) {
			c = (*x1);
			for (int j = (*y2) - 1; j >= (*y1); --j) {
				(*matrix)[l][c] = mat[j][i];
				c++;
			}
			l++;
		}
	} else if (left == 1) {
		int l = (*y1), c = (*x1);
		for (int i = (*x2) - 1; i >= (*x1); --i) {
			c = (*x1);
			for (int j = (*y1); j < (*y2); ++j) {
				(*matrix)[l][c] = mat[j][i];
				c++;
			}
			l++;
		}
	}

	// saving the bounds so they won't be lost
	if (!((*x2) - (*x1) == (*y2) - (*y1))) {
		SELECT__ALL(&(*eptr), &(*x1), &(*y1), &(*x2), &(*y2));
	} else {
		(*x1) = a;
		(*y1) = b;
		(*x2) = c;
		(*y2) = d;
	}

	// deallocating differently if the matrix is rectangular or not
	if (mat && submatrix == 1) {
		free_mem(&mat, (*eptr)->height,  &(*ok));
		(*ok) = 1;
	} else {
		free_mem(&mat, (*eptr)->width,  &(*ok));
		(*ok) = 1;
	}
}

// cropping the image and resetting the initial bounds of the image
void CROP_image(colour ***matrix, int *x1, int *y1, int *x2, int *y2,
				int *ok,  image **eptr, FILE **ptr)
{
	colour **mat;
	if (*ok == 0 || (*ptr) == 0) {
		printf("No image loaded\n");
	} else {
		printf("Image cropped\n");

		if ((*y2) != (*eptr)->height || (*x2) != (*eptr)->width ||
			(*x1) != 0 || (*y1) != 0) {
			// the new bounds of the image
			int a = (*x2) - (*x1), b = (*y2) - (*y1);

			// allocating memory for the auxiliary matrix
			alloc_mem(&mat, b, a, &(*ok));

			// copying the cropped matrix in the auxiliary
			for (int i = (*y1); i < (*y2); i++)
				for	(int j = (*x1); j < (*x2); j++)
					mat[i - (*y1)][j - (*x1)] = (*matrix)[i][j];

			// deallocating the memory for initial matrix
			if ((*matrix) != NULL) {
				free_mem(&(*matrix), (*eptr)->height, &(*ok));
				(*ok) = 1;
			}

			// copying the cropped matrix into the intial matrix
			(*matrix) = mat;

			// updating the bounds
			(*eptr)->height = b;
			(*eptr)->width = a;

			// modifying the selection with the updated bounds
			SELECT__ALL(&(*eptr), &(*x1), &(*y1), &(*x2), &(*y2));
		}
	}
}

double clamp(double x, double min, double max)
{
	if (x < min)
		x = min;
	else if (x > max)
		x = max;
	return x;
}

// function to apply the edge filter
void EDGE(colour **mat, colour ***matrix, int i, int j)
{
	double aux;

	aux = mat[i][j].red * 8 - mat[i][j - 1].red - mat[i][j + 1].red
	- mat[i + 1][j].red - mat[i - 1][j].red - mat[i - 1][j - 1].red -
	mat[i - 1][j + 1].red - mat[i + 1][j - 1].red - mat[i + 1][j + 1].red;
	aux = clamp(aux, 0, 255);
	(*matrix)[i][j].red = aux;

	aux = mat[i][j].green * 8 - mat[i][j - 1].green - mat[i][j + 1].green
	- mat[i + 1][j].green - mat[i - 1][j].green - mat[i - 1][j - 1].green -
	mat[i - 1][j + 1].green - mat[i + 1][j - 1].green - mat[i + 1][j + 1].green;
	aux = clamp(aux, 0, 255);
	(*matrix)[i][j].green = aux;

	aux = mat[i][j].blue * 8 - mat[i][j - 1].blue - mat[i][j + 1].blue
	- mat[i + 1][j].blue - mat[i - 1][j].blue - mat[i - 1][j - 1].blue -
	mat[i - 1][j + 1].blue - mat[i + 1][j - 1].blue - mat[i + 1][j + 1].blue;
	aux = clamp(aux, 0, 255);
	(*matrix)[i][j].blue = aux;
}

// function to apply the sharpen filter
void SHARPEN(colour **mat, colour ***matrix, int i, int j)
{
	double aux;

	aux = 5 * mat[i][j].red - mat[i][j - 1].red - mat[i][j + 1].red
	- mat[i + 1][j].red - mat[i - 1][j].red;
	aux = clamp(aux, 0, 255);
	(*matrix)[i][j].red = aux;

	aux = 5 * mat[i][j].green - mat[i][j - 1].green - mat[i][j + 1].green
	- mat[i + 1][j].green - mat[i - 1][j].green;
	aux = clamp(aux, 0, 255);
	(*matrix)[i][j].green = aux;

	aux = 5 * mat[i][j].blue - mat[i][j - 1].blue - mat[i][j + 1].blue
	- mat[i + 1][j].blue - mat[i - 1][j].blue;
	aux = clamp(aux, 0, 255);
	(*matrix)[i][j].blue = aux;
}

// function to apply the blur filter
void BLUR(colour **mat, colour ***matrix, int i, int j)
{
	double aux;

	aux = mat[i][j].red  + mat[i][j - 1].red + mat[i][j + 1].red
	+ mat[i + 1][j].red + mat[i - 1][j].red + mat[i - 1][j - 1].red +
	mat[i - 1][j + 1].red + mat[i + 1][j - 1].red + mat[i + 1][j + 1].red;
	aux = aux / 9;
	(*matrix)[i][j].red = aux;

	aux = mat[i][j].green  + mat[i][j - 1].green + mat[i][j + 1].green
	+ mat[i + 1][j].green + mat[i - 1][j].green + mat[i - 1][j - 1].green +
	mat[i - 1][j + 1].green + mat[i + 1][j - 1].green + mat[i + 1][j + 1].green;
	aux = aux / 9;
	(*matrix)[i][j].green = aux;

	aux = mat[i][j].blue  + mat[i][j - 1].blue + mat[i][j + 1].blue
	+ mat[i + 1][j].blue + mat[i - 1][j].blue + mat[i - 1][j - 1].blue +
	mat[i - 1][j + 1].blue + mat[i + 1][j - 1].blue + mat[i + 1][j + 1].blue;
	aux = aux / 9;
	(*matrix)[i][j].blue = aux;
}

// function to apply the gaussian blur filter
void GAUSSIAN_BLUR(colour **mat, colour ***matrix, int i, int j)
{
	double aux;
	aux = mat[i][j].red * 4  + mat[i][j - 1].red * 2 + mat[i][j + 1].red * 2
	+ mat[i + 1][j].red * 2 + mat[i - 1][j].red * 2 + mat[i - 1][j - 1].red +
	mat[i - 1][j + 1].red + mat[i + 1][j - 1].red + mat[i + 1][j + 1].red;
	aux = aux / 16;
	(*matrix)[i][j].red = aux;

	aux = mat[i][j].green * 4  + mat[i][j - 1].green * 2 + mat[i][j + 1].green
	* 2 + mat[i + 1][j].green * 2 + mat[i - 1][j].green * 2 +
	mat[i - 1][j - 1].green + mat[i - 1][j + 1].green + mat[i + 1][j - 1].green
	+ mat[i + 1][j + 1].green;
	aux = aux / 16;
	(*matrix)[i][j].green = aux;

	aux = mat[i][j].blue * 4  + mat[i][j - 1].blue * 2 + mat[i][j + 1].blue * 2
	+ mat[i + 1][j].blue * 2 + mat[i - 1][j].blue * 2 + mat[i - 1][j - 1].blue +
	mat[i - 1][j + 1].blue + mat[i + 1][j - 1].blue + mat[i + 1][j + 1].blue;
	aux = aux / 16;

	(*matrix)[i][j].blue = aux;
}

// function to apply filters for an image
void APPLY(colour ***matrix, int *x1, int *y1, int *x2, int *y2,
		   image **eptr, char **p, int *ok)
{
	colour **mat;

	// allocating memory for the auxiliary matrix
	alloc_mem(&mat, (*eptr)->height, (*eptr)->width, &(*ok));

	// copying the matrix into the auxiliary
	for (int i = 0; i < (*eptr)->height; ++i)
		for (int j = 0; j < (*eptr)->width; ++j)
			mat[i][j] = (*matrix)[i][j];

    // applying filters
	for (int i = (*y1)  ; i < (*y2)  ; i++) {
		for (int j = (*x1)  ; j < (*x2) ; j++) {
			if (i != 0 && i != (*eptr)->height - 1 &&
				j != 0 && j != (*eptr)->width - 1) {
				if (strcmp((*p), "EDGE") == 0)
					EDGE(mat, &(*matrix), i, j);
				if (strcmp((*p), "SHARPEN") == 0)
					SHARPEN(mat, &(*matrix), i, j);
				if (strcmp((*p), "BLUR") == 0)
					BLUR(mat, &(*matrix), i, j);
				if (strcmp((*p), "GAUSSIAN_BLUR") == 0)
					GAUSSIAN_BLUR(mat, &(*matrix), i, j);
			}
		}
	}

	// deallocating the memory the auxiliary
	if (mat) {
		free_mem(&mat, (*eptr)->height, &(*ok));
		(*ok) = 1;
	}
}

// function to save the matrix in an ASCII format
void SAVE_ascii(colour ***matrix, char *save, image **eptr, FILE **ptr)
{
	if ((*eptr)->height == -1 || (*eptr)->width == -1 || (*ptr) == 0) {
		printf("No image loaded\n");
	} else {
		FILE *SAVE;

		// opening the file
		SAVE = fopen(save, "w");

		// defensive programming
		if (!SAVE)
			fprintf(stderr, "Failed to open/ create file\n");

		if ((*eptr)->color == colored)
			fprintf(SAVE, "P3\n");
		else
			fprintf(SAVE, "P2\n");

		fprintf(SAVE, "%d %d\n255\n", (int)(*eptr)->width,
				(int)(*eptr)->height);

		if ((*eptr)->color == uncolored) {
			for (int i = 0; i < (*eptr)->height; i++) {
				for (int j = 0; j < (*eptr)->width; j++) {
					// casting the double value to int, rounding it and writing
					// it in the ASCII file
					fprintf(SAVE, "%d ", (int)round((*matrix)[i][j].grey));
				}
				fprintf(SAVE, "\n");
			}
		} else {
			for (int i = 0; i < (*eptr)->height; i++) {
				for (int j = 0; j < (*eptr)->width; j++) {
					// casting the double values to int, rounding them and
					// writing them in the ASCII file
					fprintf(SAVE, "%d ", (int)round((*matrix)[i][j].red));
					fprintf(SAVE, "%d ", (int)round((*matrix)[i][j].green));
					fprintf(SAVE, "%d ", (int)round((*matrix)[i][j].blue));
				}
				fprintf(SAVE, "\n");
			}
		}
		printf("Saved %s\n", save);

		// closing the file
		fclose(SAVE);
	}
}

// function to save the matrix in a BINARY format
void SAVE_binary(colour ***matrix, char *save, image **eptr, FILE **ptr)
{
	if ((*eptr)->height == -1 || (*eptr)->width == -1 || (*ptr) == 0) {
		printf("No image loaded\n");
	} else {
		FILE *SAVE;
		// opening the file
		SAVE = fopen(save, "w");

		// defensive programming and opening de file
		if (!SAVE) {
			fprintf(stderr, "Failed to open/ create file\n");
			return;
		}

		if ((*eptr)->color == colored)
			fprintf(SAVE, "P6\n");
		else
			fprintf(SAVE, "P5\n");

		fprintf(SAVE, "%d %d\n255\n", (int)(*eptr)->width,
				(int)(*eptr)->height);

		// closing the ASCII file
		fclose(SAVE);

		// opening the BINARY file
		SAVE = fopen(save, "ab");

		// defensive programming
		if ((*ptr) == NULL) {
			fprintf(stderr, "Failed to open/ create file\n");
			return;
		}

		if ((*eptr)->color == uncolored) {
			for (int i = 0; i < (*eptr)->height; i++) {
				for (int j = 0; j < (*eptr)->width; j++) {
					// casting the double value to int and rounding it
					int grey = (int)round((*matrix)[i][j].grey);

					// in order to write the value in a BINARY file we have to
					// cast it to unsigned char
					unsigned char grey2 = (unsigned char)grey;

					// writing the unsigned char value in the BINARY file
					fwrite(&grey2, 1, 1, SAVE);
				}
			}
		} else {
			for (int i = 0; i < (*eptr)->height; i++) {
				for (int j = 0; j < (*eptr)->width; j++) {
					// casting the double value to int and rounding it
					int red = (int)round((*matrix)[i][j].red);

					// in order to write the value in a BINARY file we have to
					// cast it to unsigned char
					unsigned char red2 = (unsigned char)red;

					int green = (int)round((*matrix)[i][j].green);
					unsigned char green2 = (unsigned char)green;

					int blue = (int)round((*matrix)[i][j].blue);
					unsigned char blue2 = (unsigned char)blue;

					// writing the unsigned char values in the BINARY file
					fwrite(&red2, 1, 1, SAVE);
					fwrite(&green2, 1, 1, SAVE);
					fwrite(&blue2, 1, 1, SAVE);
				}
			}
		}
		printf("Saved %s\n", save);

		// closing the file
		fclose(SAVE);
	}
}

// function to exit the program gracefully
void EXIT(FILE **ptr, colour ***matrix, int *ok, image **eptr)
{
	if ((*eptr)->width == -1 || (*eptr)->height == -1 || (*ptr) == 0) {
		fprintf(stdout, "No image loaded\n");

		exit(0);
	} else {
		// deallocating the memory for the matrix
		free_mem(&(*matrix), (*eptr)->height, &(*ok));
		(*ok) = 1;
		
		//deallocating the memory
		free(command);
		
		// closing the file
		fclose((*ptr));

		exit(0);
	}
}

// function to check if the read argument is a number
int is_number(char *str)
{
	if (!strchr("-0123456789", str[0]))
		return 0;
	for (int i = 1; str[i]; ++i)
		if (!strchr("0123456789", str[i]))
			return 0;
	return 1;
}

// function to check if the selection is correct
int invalid_select(char **p, int *a, int *b, int *c, int *d)
{
	if ((*p) == NULL) {// if the value doesn't exist, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	if (is_number((*p)) == 0) { // if is not a number, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	(*a) = atoi((*p));

	(*p) = strtok(NULL, " ");

	if ((*p) == NULL) { // if the value doesn't exist, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	if (is_number((*p)) == 0) { // if is not a number, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	(*b) = atoi((*p));

	(*p) = strtok(NULL, " ");

	if ((*p) == NULL) { // if the value doesn't exist, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	if (is_number((*p)) == 0) { // if is not a number, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	(*c) = atoi((*p));

	(*p) = strtok(NULL, " ");

	if ((*p) == NULL) { // if the value doesn't exist, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}
	if (is_number((*p)) == 0) { // if is not a number, the selection is invalid
		printf("Invalid command\n");
		return 0;
	}

	(*d) = atoi((*p));

	return 1;
}
