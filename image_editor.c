// Copyright Spinochi Andreea - 312CA

// function getline isn't defined in C, so in order to use it we have to define
// GNU SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "definitions.h"
#include "functions.h"

int main(void)
{
	int ok = 0, angle, x1 = -1, x2 = -1, y1 = -1, y2 = -1;
	image e, *eptr;
	eptr = &e;
	eptr->width = -1; eptr->height = -1;
	FILE *ptr;
	char *p, save[100], *command;
	colour **matrix;

	while (1) {
		read_command(&command);

		if (strcmp(command, "EXIT") && strlen(command) > 0)
			command[strlen(command) - 1] = '\0';

		p = strtok(command, " ");

		if (p) {
			if (strcmp(p, "LOAD") == 0) {
				p = strtok(NULL, " ");

				load_file(&p, &ptr, &eptr, &matrix, &ok, &x1, &y1, &x2, &y2);

			} else if (strcmp(p, "SELECT") == 0) {
				check_selection(&p, &eptr, &x1, &y1, &x2, &y2, &ptr);

			} else if (strcmp(p, "ROTATE") == 0) {
				p = strtok(NULL, " "); angle = atoi(p);

				ROTATE(&matrix, &eptr, angle, &x1, &y1, &x2, &y2, &ok, &ptr);

			} else if (strcmp(p, "CROP") == 0) {
				CROP_image(&matrix, &x1, &y1, &x2, &y2, &ok, &eptr, &ptr);

			} else if (strcmp(p, "APPLY") == 0) {
				check_apply(&p, &ok, &eptr, &matrix, &x1, &y1, &x2, &y2);

			} else if (strcmp(p, "EXIT") == 0) {
				EXIT(&ptr, &matrix, &ok, &eptr);

			} else if (strcmp(p, "SAVE") == 0) {
				p = strtok(NULL, " ");
				strcpy(save, p);
				p = strtok(NULL, " ");

				if (!p)
					SAVE_binary(&matrix, save, &eptr, &ptr);
				else if (strcmp(p, "ascii") == 0)
					SAVE_ascii(&matrix, save, &eptr, &ptr);

			} else {
				printf("Invalid command\n");
			}
		}
		
	}
	return 0;
}
