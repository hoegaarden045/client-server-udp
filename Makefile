
build: clean
	gcc server.c -o server
	gcc client.c -o client

clean:
	@rm -vf client server

