MODULES = ../../modules

INCLUDE = ../../include

CC = gcc

CFLAGS = -Wall -g -I$(INCLUDE) -Werror

OBJS = list_insert_next_steps.o $(MODULES)/UsingLinkedList/ADTList.o

EXEC = list_insert_next_steps

ARG1 = real > real.csv

ARG2 = amortized > amortized.csv

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARG1)
	./$(EXEC) $(ARG2)

valgrind: $(EXEC)
	valgrind ./$(EXEC) $(ARGS)