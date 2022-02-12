CC := gcc

OBJS := stack.o stats.o


app.exe : $(OBJS) main.o
	$(CC) $^ -o $@

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<
	
run: app.exe
	./app.exe .

clean: 
	rm *.o *.exe