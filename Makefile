gll:
	gcc *.c -o main.o

	./main.o hello.ll

	cat out.s var.s > gll.s

	nasm -f elf64 gll.s 

	gcc -no-pie gll.o -o gll

	rm *.o var.s out.s

test:
	make gll

	./gll

testASM:
	nasm -f elf64 testcode/test.asm

	gcc -no-pie testcode/test.o -o testcode/test

	rm testcode/*.o

	./testcode/test


