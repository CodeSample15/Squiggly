CC = g++

SRC = src/
INCL = include/
OBJ = objs/

all : ${OBJ}squiggly.o ${OBJ}tokenizer.o ${OBJ}linter.o ${OBJ}runner.o ${OBJ}built-in-funcs.o ${OBJ}utils.o ${OBJ}external-libs.o
	$(CC) -Wall -Werror -I$(INCL) -o squiggly ${OBJ}squiggly.o ${OBJ}tokenizer.o ${OBJ}linter.o ${OBJ}runner.o ${OBJ}built-in-funcs.o ${OBJ}utils.o ${OBJ}external-libs.o

${OBJ}%.o : ${SRC}%.cpp
	@if [ ! -d ${OBJ} ]; then \
		mkdir ${OBJ}; \
	fi

	$(CC) -Wall -Werror -I$(INCL) -c $< -Wa,-mbig-obj  -o $@

clean :
	rm -f squiggly
	rm -rf ${OBJ}*.o