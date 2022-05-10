#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILENAME "outfile.txt"

void print_file_info(char *message)
{
    struct stat statbuf;
    printf("%s", message);
    if (stat(FILENAME , &statbuf) == 0)
    {
        printf("st_ino: %ld\n", statbuf.st_ino);
        printf("st_size: %ld\n", statbuf.st_size);
        printf("st_blksize: %ld\n\n", statbuf.st_blksize);
    }
    else
        printf("Error in stat\n\n");
}

int main()
{
    print_file_info("Before first open\n");
    FILE *f1 = fopen(FILENAME, "a");
    print_file_info("After first open\n");
    FILE *f2 = fopen(FILENAME, "a");
    print_file_info("After second open\n");

    for (char c = 'a'; c <= 'z'; c++)
    {
        if (c % 2)
        {
            fprintf(f1, "%c", c);
        }
        else
        {
            fprintf(f2, "%c", c);
        }
    }

    print_file_info("Before first close\n");
    fclose(f1);
    print_file_info("After first close\n");
    fclose(f2);
    print_file_info("After second close\n");

    return 0;
}
