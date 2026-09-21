main:main.o thread_pool.o
	gcc main.o thread_pool.o -o main  -pthread

main.o:main.c thread_pool.h
	gcc -c main.c -o main.o -pthread

thread_pool.o:thread_pool.c thread_pool.h
	gcc -c thread_pool.c -o thread_pool.o -pthread 

