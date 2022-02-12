CC=gcc
FLAGS=-Wall -Werror -pthread

app.exe:
	$(CC) $(FLAGS) lab_01.c -o app.exe

run: app.exe
	sudo ./app.exe

log:
	sudo cat /var/log/syslog

signal:
	sudo kill -9 pid

clean:
	rm -f *.exe *.o