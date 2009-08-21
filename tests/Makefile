include ../Makefile.in

TEST_CFLAGS = $(CFLAGS) -I../src
TEST_LIB_FLAGS = $(LIB_FLAGS) -L../ -lpoison -lcunit

OBJS = test_session.o test_dhcp.o main.o

all: tests
	@LD_LIBRARY_PATH=".." ./tests

tests: $(OBJS)
	$(CC) $(TEST_CFLAGS) $(TEST_LIB_FLAGS) $(OBJS) -o tests

main.o: main.c
	$(CC) $(TEST_CFLAGS) -c main.c -o main.o

test_session.o: test_session.c
	$(CC) $(TEST_CFLAGS) -c test_session.c -o test_session.o

test_dhcp.o: test_dhcp.c
	$(CC) $(TEST_CFLAGS) -c test_dhcp.c -o test_dhcp.o

clean:
	rm -f $(OBJS) tests
