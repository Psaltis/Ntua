CC = gcc
OBJ = strcmp.s strcpy.s strcat.s strlen.s
string_manipulation.out: string_manipulation.o strcmp.o strcpy.o
strcat.o strlen.o
$(CC) $^ -o $@
string_manipulation.o: string_manipulation.c
$(CC) -c $^
%.o: $(OBJ).s
$(CC) -c $<
clean:
rm -f *.o *.out
