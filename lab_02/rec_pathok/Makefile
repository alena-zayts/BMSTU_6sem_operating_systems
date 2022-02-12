CC := gcc


app.exe :  recursive.o
	$(CC) $^ -o $@

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<
	
run: app.exe
	./app.exe .

clean: 
	rm *.o *.exe