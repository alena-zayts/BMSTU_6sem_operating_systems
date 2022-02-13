app.exe:
	gcc -Wall -Werror -pthread lab_01.c -o app.exe

run: app.exe
	sudo ./app.exe

log:
	sudo cat /var/log/syslog | grep "my_daemon"

signal:
	sudo kill -9 pid

clean:
	rm -f *.exe *.o