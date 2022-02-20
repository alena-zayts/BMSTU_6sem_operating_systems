CC := gcc


app.exe :  recursive.o
	$(CC) $^ -o $@

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<
	
run: app.exe
	./app.exe ../..

clean: 
	rm *.o *.exe test_fifo test.txt test_soft_link

files:
	mknod test_fifo p
	touch test.txt
	ln -s test.txt test_soft_link
	