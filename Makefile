CC = gcc
CFLAGS = -Wall -O2 -ggdb
LIB_FLAGS = -lpcap -lnet

OBJS = src/poison_host.o src/poison_target.o src/poison_trigger.o src/poison_session.o src/poison_arp.o src/poison_dhcp.o src/poison_init.o
HEADERS = src/poison_types.h src/poison_host.h src/poison_target.h src/poison_trigger.h src/poison_session.h src/poison_arp.h src/poison_dhcp.h src/poison_init.h

LIB_DIR = /usr/lib/
INCLUDE_DIR = /usr/include/

all: libpoison.so

libpoison.so: $(OBJS)
	gcc $(CFLAGS) -shared $(LIB_FLAGS) $(OBJS) -o libpoison.so

src/poison_host.o: src/poison_host.h src/poison_host.c
	gcc $(CFLAGS) -c src/poison_host.c -o src/poison_host.o

src/poison_init.o: src/poison_init.h src/poison_init.c
	gcc $(CFLAGS) -c src/poison_init.c -o src/poison_init.o

src/poison_target.o: src/poison_target.h src/poison_target.c
	gcc $(CFLAGS) -c src/poison_target.c -o src/poison_target.o

src/poison_trigger.o: src/poison_trigger.h src/poison_trigger.c
	gcc $(CFLAGS) -c src/poison_trigger.c -o src/poison_trigger.o

src/poison_session.o: src/poison_session.h src/poison_session.c
	gcc $(CFLAGS) -c src/poison_session.c -o src/poison_session.o

src/poison_dhcp.o: src/poison_dhcp.h src/poison_dhcp.c
	gcc $(CFLAGS) -c src/poison_dhcp.c -o src/poison_dhcp.o

src/poison_arp.o: src/poison_arp.h src/poison_arp.c
	gcc $(CFLAGS) -c src/poison_arp.c -o src/poison_arp.o

clean:
	rm -rf $(OBJS) libpoison.so

install: libpoison.so $(HEADERS)
	cp libpoison.so $(LIB_DIR)
	cp libpoison.h $(INCLUDE_DIR)
	mkdir -p $(INCLUDE_DIR)/libpoison
#	for $$i in $(HEADERS) do; cp $$i $(INCLUDE_DIR)/libpoison/; done
	cp src/*.h $(INCLUDE_DIR)/libpoison/
