.PHONY: all, run, clean

run: all
clean:
	rm ./client
	rm ./server
all:
	gcc client.c -lpthread -o client
	gcc server.c -lpthread -o server