//Second realization

int main(int argc, char ** argv)
{
    int i;
    int fd;
    unsigned char buf[sizeof(unsigned int)];
    if (argc != 2)
    {
        return 0;
    }
    fd = open(argv[1],O_RDWR);
    if (fd == -1)
    {
        return 0;
    }
    while (read(fd,buf,sizeof(unsigned int)) > 0)
    {
        for (i = 0; i < sizeof(unsigned int); i++)
        {
            buf[i] = ((buf[i] >> 4) & 0x0f) | ((buf[i] << 4) & 0xf0);
        }
        lseek(fd,-sizeof(unsigned int),SEEK_CUR);
        write(fd,buf,sizeof(unsigned int));
    }
    return 0;
}
