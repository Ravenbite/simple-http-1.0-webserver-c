DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS = -g3 -Wall $(LIBS)
	EXECUTABLE = server
else
    CFLAGS = -O2 -Wall $(LIBS)
	EXECUTABLE = server
endif

CC = gcc $(CFLAGS)
OBJECTS = main.c ./serving/serving_interface.c ./serving/serv_respond.c ./serving/serv_route.c ./serving/serv_respond_alone.c ./serving/serv_respond_cached.c ./serving/serv_receive.c ./serving/serv_objects.c ./serving/serv_fileIO.c ./auxiliary/aux_sharedFx.c ./caching/cache_double_linked_list.c ./caching/cache_double_linked_node.c ./caching/cache_hashTable.c
LIBS = -pthread
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -c $<