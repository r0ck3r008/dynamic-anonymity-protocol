LINK_FLAGS=-pthread -lsodium -lcrypto -g -o 
ALL_OBJS= main.o end_db_connection.o cli_run.o cli_server.o handle_new_client.o connect_by_addr.o update_clients_table.o peer_run.o peer_server.o server_workings.o server_init.o update_existance.o snd_rcv.o regex_check.o dbconnect.o gen_keys.o allocate.o

all: $(PEER_BIN) clean_objects

$(PEER_BIN): $(ALL_OBJS)
	$(COMPILER) -I$(I_PATH) $(ALL_OBJS) $(LINK_FLAGS) $(PEER_BIN)

main.o: main.c
	$(COMPILER) -I$(I_PATH) -c main.c

end_db_connection.o: end_db_connection.c
	$(COMPILER) -I$(I_PATH) -c end_db_connection.c

cli_run.o: cli_run.c
	$(COMPILER) -I$(I_PATH) -c cli_run.c

cli_server.o: cli_server.c
	$(COMPILER) -I$(I_PATH) -c cli_server.c

handle_new_client.o: handle_new_client.c
	$(COMPILER) -I$(I_PATH) -c handle_new_client.c

connect_by_addr.o: connect_by_addr.c
	$(COMPILER) -I$(I_PATH) -c connect_by_addr.c

update_clients_table.o: update_clients_table.c
	$(COMPILER) -I$(I_PATH) -c update_clients_table.c

peer_run.o: peer_run.c
	$(COMPILER) -I$(I_PATH) -c peer_run.c

peer_server.o: peer_server.c
	$(COMPILER) -I$(I_PATH) -c peer_server.c

server_workings.o: server_workings.c
	$(COMPILER) -I$(I_PATH) -c server_workings.c

server_init.o: server_init.c
	$(COMPILER) -I$(I_PATH) -c server_init.c

update_existance.o: update_existance.c
	$(COMPILER) -I$(I_PATH) -c update_existance.c

snd_rcv.o: snd_rcv.c
	$(COMPILER) -I$(I_PATH) -c snd_rcv.c

regex_check.o: regex_check.c
	$(COMPILER) -I$(I_PATH) -c regex_check.c

dbconnect.o: $(I_PATH)/common_sources/dbconnect.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/dbconnect.c

gen_keys.o: $(I_PATH)/common_sources/gen_keys.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/gen_keys.c

allocate.o: allocate.c
	$(COMPILER) -I$(I_PATH) -c allocate.c

clean_objects:
	rm *.o
