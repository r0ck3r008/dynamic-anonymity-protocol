I_PATH=$(HOME)/git/dynamic-anonimity-protocol
COMPILER=gcc
LINK_FLAGS=-pthread -lsodium -lcrypto -g -o
FINAL_BIN=client.elf
ALL_OBJS=main.o end_db_connection.o transactions.o authenticate_with_const_peer.o authenticate_with_tmp_peer.o get_connect_to_new_peer.o connect_to_new_peer.o get_rand_peer.o get_rand_sno.o write_key_to_file.o dbconnect.o gen_keys.o  encrypt_snd_rcv.o snd_rcv.o allocate.o

all: $(FINAL_BIN) clean_objects

$(FINAL_BIN): $(ALL_OBJS)
	$(COMPILER) -I$(I_PATH) $(ALL_OBJS) $(LINK_FLAGS) $(FINAL_BIN) 

main.o: main.c
	$(COMPILER) -I$(I_PATH) -c main.c

end_db_connection.o: end_db_connection.c
	$(COMPILER) -I$(I_PATH) -c end_db_connection.c

transactions.o: transactions.c
	$(COMPILER) -I$(I_PATH) -c transactions.c

authenticate_with_const_peer.o: authenticate_with_const_peer.c
	$(COMPILER) -I$(I_PATH) -c authenticate_with_const_peer.c

authenticate_with_tmp_peer.o: authenticate_with_tmp_peer.c
	$(COMPILER) -I$(I_PATH) -c authenticate_with_tmp_peer.c

get_connect_to_new_peer.o: get_connect_to_new_peer.c
	$(COMPILER) -I$(I_PATH) -c get_connect_to_new_peer.c

connect_to_new_peer.o: connect_to_new_peer.c
	$(COMPILER) -I$(I_PATH) -c connect_to_new_peer.c

get_rand_peer.o: get_rand_peer.c
	$(COMPILER) -I$(I_PATH) -c get_rand_peer.c

get_rand_sno.o: get_rand_sno.c
	$(COMPILER) -I$(I_PATH) -c get_rand_sno.c

write_key_to_file.o: write_key_to_file.c
	$(COMPILER) -I$(I_PATH) -c write_key_to_file.c

dbconnect.o: $(I_PATH)/common_sources/dbconnect.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/dbconnect.c

encrypt_snd_rcv.o: encrypt_snd_rcv.c
	$(COMPILER) -I$(I_PATH) -c encrypt_snd_rcv.c

snd_rcv.o: $(I_PATH)/common_sources/snd_rcv.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/snd_rcv.c

gen_keys.o: $(I_PATH)/common_sources/gen_keys.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/gen_keys.c

allocate.o: $(I_PATH)/common_sources/allocate.c
	$(COMPILER) -I$(I_PATH) -c $(I_PATH)/common_sources/allocate.c

clean_objects:
	rm *.o

clean_bin:
	rm $(FINAL_BIN)
