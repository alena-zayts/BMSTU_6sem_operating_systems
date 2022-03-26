app.o:
	gcc -c main.c

app.exe: app.o
	gcc -Wall -Werror main.c -o app.exe

run: app.exe
	./app.exe self
