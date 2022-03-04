CC=gcc
CFLAGS=-Wall -Wextra -std=c99

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
TARGETS=$(OBJS:%.o=%)

build: 
	gcc -Wall -Wextra -std=c99 *.c -o image_editor -lm

pack:
	zip -FSr 312CA_SpinochiAndreea_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS) $(OBJS)

.PHONY: pack clean
