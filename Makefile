gll:
	gcc *.c -o main.o

	./main.o hello.ll

	cat out.s var.s > gll.s

	nasm -f elf64 gll.s 

	gcc -no-pie gll.o -o gll

	rm *.o 

	rm *.s

test:
	gcc *.c -o main.o -Wall

	./main.o hello.ll

	cat preamble.s functions.s main.s var.s > gll.s

	nasm -f elf64 gll.s 

	gcc -no-pie gll.o -o gll

	rm *.o
	
	find . -maxdepth 1 -name "*.s" ! -name "gll.s" ! -name "functions.s" -delete

	./gll

testASM:
	nasm -f elf64 testcode/test.asm

	gcc -no-pie testcode/test.o -o testcode/test

	rm testcode/*.o

	./testcode/test


