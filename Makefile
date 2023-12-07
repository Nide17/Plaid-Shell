
# Create a Makefile;
# be certain to set the following flags to gcc : -Wall - Werror - g - fsanitize = address
CFLAGS=-Wall -Werror -g -fsanitize=address
TARGETS=plaid tokenize_test
OBJS=clist.o tokenize.o
HDRS=clist.h token.h tokenize.h
LIBS=-lasan -lm -lreadline

all: $(TARGETS)

plaid: $(OBJS) plaid.o
	gcc $(LDFLAGS) $^ $(LIBS) -o $@

tokenize_test: $(OBJS) tokenize_test.o
	gcc $(LDFLAGS) $^ $(LIBS) -o $@

%.o: %.c $(HDRS)
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGETS)