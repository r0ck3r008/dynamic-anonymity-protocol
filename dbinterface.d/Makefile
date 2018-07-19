LINK_FLAGS=-pthread -lsodium `pkg-config --cflags mariadb --libs mariadb` -g -o
ALL_OBJS= main.o peer_run.o peer_server.o cli_run.o cli_server.o query_db.o server_workings.o server_init.o dbconnect.o snd_rcv.o get_passwd.o allocate.o

all: $(DBINTERFACE_BIN) clean_objects

$(DBINTERFACE_BIN): $(ALL_OBJS)
	$(COMPILER) -I$(I_PATH) $(ALL_OBJS) $(LINK_FLAGS) $(DBINTERFACE_BIN)

main.o: main.c
	$(COMPILER) -I$(I_PATH) -c main.c

peer_run.o: peer_run.c
	$(COMPILER) -I$(I_PATH) -c peer_run.c

peer_server.o: peer_server.c
	$(COMPILER) -I$(I_PATH) -c peer_server.c

cli_run.o: cli_run.c
	$(COMPILER) -I$(I_PATH) -c cli_run.c

cli_server.o: cli_server.c
	$(COMPILER) -I$(I_PATH) -c cli_server.c

query_db.o: query_db.c
	$(COMPILER) -I$(I_PATH) -c query_db.c

server_workings.o: server_workings.c
	$(COMPILER) -I$(I_PATH) -c server_workings.c

server_init.o: server_init.c
	$(COMPILER) -I$(I_PATH) -c server_init.c

dbconnect.o: dbconnect.c
	$(COMPILER) -I$(I_PATH) -c dbconnect.c

snd_rcv.o: $(I_PATH)/common_sources/snd_rcv.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/snd_rcv.c

get_passwd.o: get_passwd.c
	$(COMPILER) -I$(I_PATH) -c get_passwd.c

allocate.o: allocate.c
	$(COMPILER) -I$(I_PATH) -c allocate.c

clean_objects:
	rm *.o
