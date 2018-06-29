all: dbinterface.elf clean_objects

dbinterface.elf: main.o peer_run.o peer_server.o cli_run.o cli_server.o query_db.o server_workings.o server_init.o dbconnect.o snd_rcv.o get_passwd.o allocate.o
	gcc -pthread -lsodium `pkg-config --cflags mariadb --libs mariadb` -g  main.o peer_run.o peer_server.o cli_run.o cli_server.o query_db.o server_workings.o server_init.o dbconnect.o snd_rcv.o get_passwd.o allocate.o -o dbinterface.elf

main.o: main.c
	gcc -c main.c

peer_run.o: peer_run.c
	gcc -c peer_run.c

peer_server.o: peer_server.c
	gcc -c peer_server.c

cli_run.o: cli_run.c
	gcc -c cli_run.c

cli_server.o: cli_server.c
	gcc -c cli_server.c

query_db.o: query_db.c
	gcc -c query_db.c

server_workings.o: server_workings.c
	gcc -c server_workings.c

server_init.o: server_init.c
	gcc -c server_init.c

dbconnect.o: dbconnect.c
	gcc -c dbconnect.c

snd_rcv.o: snd_rcv.c
	gcc -c snd_rcv.c

get_passwd.o: get_passwd.c
	gcc -c get_passwd.c

allocate.o: allocate.c
	gcc -c allocate.c

clean_objects:
	rm *.o

clean_bin:
	rm dbinterface.elf
