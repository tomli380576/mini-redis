clean:
	rm -f *.o server client

server:
	gcc -Wall -c server.c 
	gcc -Wall -c read_write_helpers.c
	gcc -Wall -o server server.o read_write_helpers.o 

client:
	gcc -Wall -c client.c
	gcc -Wall -c read_write_helpers.c
	gcc -Wall -o client client.o read_write_helpers.o 