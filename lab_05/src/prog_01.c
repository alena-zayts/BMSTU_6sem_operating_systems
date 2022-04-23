#include <stdio.h>
#include <fcntl.h>

int main(void)
{
	// have kernel open connection to file alphabet.txt
    int fd = open("alphabet.txt", O_RDONLY);

	// create two C I/O buffered streams using the above connection 
    FILE *fs1 = fdopen(fd, "r");
    char buff1[20];
    setvbuf(fs1, buff1, _IOFBF, 20);

    FILE *fs2 = fdopen(fd, "r");
    char buff2[20];
    setvbuf(fs2, buff2, _IOFBF, 20);

	// read a char & write it alternatingly from fs1 and fs2
    int flag1 = 1, flag2 = 2;
    while (flag1 == 1 || flag2 == 1)
    {
        char c;

        if ((flag1 = fscanf(fs1, "%c", &c)) == 1)
        {
            fprintf(stdout, "%c", c);
        }

        if ((flag2 = fscanf(fs2, "%c", &c)) == 1)
        {
            fprintf(stdout, "%c", c);
        }
    }

    return 0;
}

/*
Функция fdopen связывает поток с существующим описателем файла fildes. Режим mode потока (одно из следующих значений: "r", "r+", "w", "w+", "a", "a+") 
должен быть совместим с режимом описателя файла. Указатель файловой позиции в новом потоке принимает значение, равное значению fildes, 
а указатели ошибок и конца файла по значению равны нулю. Режимы "w" или "w+" не "урезают" файл. Описатель файла не скопирован и будет закрыт,
 когда поток, созданный fdopen, закрывается. Результат применения fdopen в общем объекте памяти неопределен.

Существует три типа буферизации: нулевая буферизация (ее отсутствие), буферизация блока и буферизация строки.
Функция setvbuf может быть использована над любым открытым потоком для изменения типа буферизации. Параметр mode должен быть одним из трех следующих макросов:

_IONBF
(отключить буферизацию);
_IOLBF
(строчная буферизация);
_IOFBF
(блочная буферизация).
*/
