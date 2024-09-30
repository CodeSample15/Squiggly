CC = g++

SRC = src/
INCL = include/

all : squiggly.o tokenizer.o
	$(CC) -Wall -Werror -I$(INCL) -o squiggly squiggly.o tokenizer.o 
	rm -f *.o	

%.o : ${SRC}%.cpp
	$(CC) -Wall -Werror -I$(INCL) -c $< -o $@

clean :
	rm -rf squiggly
	rm -f *.o