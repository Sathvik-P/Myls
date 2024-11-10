#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>


void printLinkNType(struct dirent* file, const char* path);

int main(int argc, char *argv[])
{
	const char* directory  = (argc > 1) ? argv[1] : ".";
	DIR *dir = opendir(directory);

	if (dir == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	struct dirent* file =  readdir(dir);
	while (file != NULL)
	{
		if (errno != 0)
		{
			perror("readdir");
			exit(EXIT_FAILURE);
		}	
		printf("%16.16s\t", file->d_name);
		printLinkNType(file, directory);
		printf("\n");
		file = readdir(dir);
			
	}

	closedir(dir);

return 0;

}


const char* getFileType(mode_t mode)
{
	switch (mode & S_IFMT)
	{
		case S_IFREG: return "REG";
		case S_IFDIR: return "DIR";
		case S_IFSOCK: return "SOCK";
		case S_IFLNK: return "LNK";
		case S_IFBLK: return "BLK";
		case S_IFCHR: return "CHR";
		case S_IFIFO: return "FIFO";
		default: return "UKNOWN";
	}
}


void printLinkNType(struct dirent* file, const char* path)
{
	struct stat fileStat;
	char fullPath[PATH_MAX];
	char absolutePath[PATH_MAX];
	snprintf(fullPath, sizeof(fullPath), "%s/%s", path, file->d_name);
	//stat(fullPath, &fileStat);
	
	//printf("%s", fullPath);

	if (stat(fullPath, &fileStat) == -1)
	{
	//	printf("I am failing here");
		perror("stat");
		exit(EXIT_FAILURE);
	}
	
	printf("%" PRIdMAX "\t %s", fileStat.st_nlink, getFileType(fileStat.st_mode));
	
	if (S_ISDIR(fileStat.st_mode))
	{
		DIR *subdir = opendir(fullPath);
		if (subdir == NULL)
			printf("\tunknown");
		else
		{
			int entry = 0;
			struct dirent *subEntry;
			while ((subEntry = readdir(subdir)) != NULL)
				entry++;
			closedir(subdir);
			printf("\t%d", entry);
		}

	}
	else
		printf("\t%" PRIdMAX "\t", fileStat.st_size);


	if (S_ISREG(fileStat.st_mode))
	{
		int inputFd;
		char buffer[17];
		ssize_t numRead;

		inputFd = open(fullPath, O_RDONLY);
		if (inputFd == -1)
		{
			perror("open");
			exit(EXIT_FAILURE);
		}

		numRead = read(inputFd, buffer, 16);
		buffer[16] = '\0';
		for (int i = 0; i < 16; i++)
		{
			if (!isprint(buffer[i]) || !isascii(buffer[i]))
					buffer[i] = ' ';
		}

		printf("%s", buffer);
		close(inputFd);
	}	


}





