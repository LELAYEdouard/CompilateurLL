test:
	gcc *.c -o main.o

	./main.o hello.ll

	nasm -f elf64 out.s 

	gcc -no-pie out.o -o out

	rm *.o

	./out

testASM:
	nasm -f elf64 testcode/test.asm

	gcc -no-pie testcode/test.o -o testcode/test

	rm testcode/*.o

	./testcode/test


