all: server.elf

server.elf: main.o server_init.o server_workings.o snd_rcv.o allocate.o
	gcc -pthread -g main.o server_init.o server_workings.o snd_rcv.o allocate.o -o server.elf

main.o: main.c
	gcc -c main.c

server_workings.o: server_workings.c
	gcc -c server_workings.c

server_init.o: server_init.c
	gcc -c server_init.c

snd_rcv.o: snd_rcv.c
	gcc -c snd_rcv.c

allocate.o: allocate.c
	gcc -c allocate.c

clean_objects:
	rm *.o

clean_bin:
	rm server.elf
