CC = gcc
CFLAGS = -Wall
DEPS = CIHeader.h
OBJ = ControlInterface.o CIHeader.o

%.o: %.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

CI: $(OBJ)
		gcc $(CFLAGS) -o $@ $^