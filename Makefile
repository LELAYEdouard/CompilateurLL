test:
	gcc *.c -o main.o

	./main.o hello.ll

	nasm -f elf64 out.s 

	gcc -no-pie out.o -o out

	rm *.o

	./out

