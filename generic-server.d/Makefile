I_PATH=$(HOME)/git/dynamic-anonimity-protocol
COMPILER=gcc
LINK_FLAGS=-pthread -g -o
FINAL_BIN=server.elf
ALL_OBJS= main.o server_init.o server_workings.o snd_rcv.o allocate.o

all: $(FINAL_BIN) clean_objects

$(FINAL_BIN): $(ALL_OBJS)
	$(COMPILER) -I$(I_PATH) $(ALL_OBJS) $(LINK_FLAGS) $(FINAL_BIN)

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

clean_bin:
	rm $(FINAL_BIN)
