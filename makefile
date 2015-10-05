proc=1

all: make

make: 
	mkdir out
	gcc source/server.c -pthread -Dproc=$(proc) -o out/server.out
	gcc source/client.c -o out/client.out
clean:
	rm -rf out
