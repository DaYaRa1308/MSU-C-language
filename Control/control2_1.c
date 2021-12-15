#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
    int i;
    int fd;
    unsigned char buf;
    if (argc != 2)
    {
        return 0;
    }
    fd = open(argv[1],O_RDWR);
    if (fd == -1)
    {
        return 0;
    }
    while (read(fd,&buf,sizeof(unsigned char)) > 0)
    {
        buf = ((buf >> 4) & 0x0f) | ((buf << 4) & 0xf0);
        lseek(fd,-sizeof(unsigned char),SEEK_CUR);
        write(fd,&buf,sizeof(unsigned char));
    }
    return 0;
} 
