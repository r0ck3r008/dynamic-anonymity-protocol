all: client.elf

client.elf: main.o end_db_connection.o transactions.o authenticate_with_const_peer.o authenticate_with_tmp_peer.o get_connect_to_new_peer.o connect_to_new_peer.o get_rand_peer.o get_rand_sno.o write_key_to_file.o dbconnect.o gen_keys.o  encrypt_snd_rcv.o snd_rcv.o allocate.o
	gcc -pthread -lsodium -lcrypto -g main.o end_db_connection.o transactions.o authenticate_with_const_peer.o authenticate_with_tmp_peer.o get_connect_to_new_peer.o connect_to_new_peer.o get_rand_peer.o get_rand_sno.o write_key_to_file.o dbconnect.o gen_keys.o encrypt_snd_rcv.o snd_rcv.o allocate.o -o client.elf
 

main.o: main.c
	gcc -c main.c

end_db_connection: end_db_connection.c
	gcc -c end_db_connection.c

transactions.o: transactions.c
	gcc -c transactions.c

authenticate_with_const_peer.o: authenticate_with_const_peer.c
	gcc -c authenticate_with_const_peer.c

authenticate_with_tmp_peer.o: authenticate_with_tmp_peer.c
	gcc -c authenticate_with_tmp_peer.c

get_connect_to_new_peer.o: get_connect_to_new_peer.c
	gcc -c get_connect_to_new_peer.c

connect_to_new_peer.o: connect_to_new_peer.c
	gcc -c connect_to_new_peer.c

get_rand_peer.o: get_rand_peer.c
	gcc -c get_rand_peer.c

get_rand_sno.o: get_rand_sno.c
	gcc -c get_rand_sno.c

write_key_to_file.o: write_key_to_file.c
	gcc -c write_key_to_file.c

dbconnect.o: dbconnect.c
	gcc -c dbconnect.c

encrypt_snd_rcv.o: encrypt_snd_rcv.c
	gcc -c encrypt_snd_rcv.c

snd_rcv.o: snd_rcv.c
	gcc -c snd_rcv.c

gen_keys.o: gen_keys.c
	gcc -c gen_keys.c

allocate.o: allocate.c
	gcc -c allocate.c

clean_objects:
	rm *.o

clean_bin:
	rm client.elf
