CC = g++

SRC = src/
INCL = include/
OBJ = objs/

all : ${OBJ}squiggly.o ${OBJ}tokenizer.o ${OBJ}linter.o ${OBJ}runner.o ${OBJ}built-in-funcs.o
	$(CC) -Wall -Werror -I$(INCL) -o squiggly ${OBJ}squiggly.o ${OBJ}tokenizer.o ${OBJ}linter.o ${OBJ}runner.o ${OBJ}built-in-funcs.o

${OBJ}%.o : ${SRC}%.cpp
	@if [ ! -d ${OBJ} ]; then \
		mkdir ${OBJ}; \
	fi

	$(CC) -Wall -Werror -I$(INCL) -c $< -o $@

clean :
	rm -f squiggly
	rm -rf ${OBJ}*.o