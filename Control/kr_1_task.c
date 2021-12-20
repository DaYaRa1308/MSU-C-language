int main(int argc,char ** argv)
{
	if (argc != 2)
	{
		return 1;
	}
	int fd = open(argv[1],O_RDWR);
	if (fd == -1)
	{
		return 1;
	}
	int bytes = 0;
	int num;
	char c;
	for(;;)
	{
		ans1 = read(fd,&c,sizeof(char));
		if (ans1 == 0)
		{
			break;
		}
		bytes += ans1;
	}
	if ((bytes % (sizeof(int)+sizeof(char))) != 0)
	{
		return 1;
	}
	lseek(fd,0,SEEK_SET);
	for(;;)
	{
		ans1 = read(fd,&c,sizeof(char));
		ans2 = read(fd,&num,sizeof(int));
		if (ans1 == 0 || ans2 == 0)
		{
			break;
		}
		lseek(fd,-(sizeof(int)+sizeof(char)),SEEK_CUR);
		write(fd,&num,sizeof(int));
		write(fd,&c,sizeof(char));
	}
	return 0;
}
