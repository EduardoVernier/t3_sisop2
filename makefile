make:
	gcc -o server server.c -pthread
	gcc -o client client.c -pthread
	gcc -o screen screen.c
