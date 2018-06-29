all: peer.elf clean_objects

peer.elf: main.o end_db_connection.o cli_run.o cli_server.o handle_new_client.o connect_by_addr.o update_clients_table.o peer_run.o peer_server.o server_workings.o server_init.o update_existance.o snd_rcv.o regex_check.o dbconnect.o gen_keys.o allocate.o
	gcc -pthread -lsodium -lcrypto -g  main.o end_db_connection.o cli_run.o cli_server.o handle_new_client.o connect_by_addr.o update_clients_table.o peer_run.o peer_server.o server_workings.o server_init.o update_existance.o snd_rcv.o regex_check.o dbconnect.o gen_keys.o allocate.o -o peer.elf

main.o: main.c
	gcc -c main.c

end_db_connection.o: end_db_connection.c
	gcc -c end_db_connection.c

cli_run.o: cli_run.c
	gcc -c cli_run.c

cli_server.o: cli_server.c
	gcc -c cli_server.c

handle_new_client.o: handle_new_client.c
	gcc -c handle_new_client.c

connect_by_addr.o: connect_by_addr.c
	gcc -c connect_by_addr.c

update_clients_table.o: update_clients_table.c
	gcc -c update_clients_table.c

peer_run.o: peer_run.c
	gcc -c peer_run.c

peer_server.o: peer_server.c
	gcc -c peer_server.c

server_workings.o: server_workings.c
	gcc -c server_workings.c

server_init.o: server_init.c
	gcc -c server_init.c

update_existance.o: update_existance.c
	gcc -c update_existance.c

snd_rcv.o: snd_rcv.c
	gcc -c snd_rcv.c

regex_check.o: regex_check.c
	gcc -c regex_check.c

dbconnect.o: dbconnect.c
	gcc -c dbconnect.c

gen_keys.o: gen_keys.c
	gcc -c gen_keys.c

allocate.o: allocate.c
	gcc -c allocate.c

clean_objects:
	rm *.o

clean_bin:
	rm peer.elf
