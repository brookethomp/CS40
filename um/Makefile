# Makefile for CS40 HW6: UM

CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -l40locality -lcii40 -lm -lum-dis -lcii
INCLUDES = $(shell echo *.h)

EXECS: um

all: $(EXECS)

writetests: umlabwrite.o umlab.o umarithmetic.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)



# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

um: ummem.o um.o umarithmetic.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(EXECS) *.o