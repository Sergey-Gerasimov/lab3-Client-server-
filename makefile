proc=1

all: make

make: 
	mkdir out
	gcc source/server.c -Dproc=$(proc) -o out/client.out
	gcc source/client.c -o out/server.out
clean:
	rm -rf out
