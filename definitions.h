// Copyright Spinochi Andreea - 312CA

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// enumeration used to check if the image is colored or not
enum colors {colored = 1, uncolored = 0};

// struct that contains the initial values of the image
typedef struct {
	double width, height;
	char magic_word[2];
	double max_value;
	int color;
} image;

// struct that contains the values of the matrices
typedef struct {
	double grey, red, green, blue;
} colour;

#endif
