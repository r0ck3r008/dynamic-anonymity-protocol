LINK_FLAGS=-pthread -g -o
ALL_OBJS= main.o server_init.o server_workings.o snd_rcv.o allocate.o

all: $(SERVER_BIN) clean_objects

$(SERVER_BIN): $(ALL_OBJS)
	$(COMPILER) -I$(I_PATH) $(ALL_OBJS) $(LINK_FLAGS) $(SERVER_BIN)

main.o: main.c
	$(COMPILER) -I$(I_PATH) -c main.c

server_workings.o: server_workings.c
	$(COMPILER) -I$(I_PATH) -c server_workings.c

server_init.o: server_init.c
	$(COMPILER) -I$(I_PATH) -c server_init.c

snd_rcv.o: $(I_PATH)/common_sources/snd_rcv.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/snd_rcv.c

allocate.o: $(I_PATH)/common_sources/allocate.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/allocate.c

clean_objects:
	rm *.o
