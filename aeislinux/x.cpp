#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int createDirectories(char * szDestinationDir);
int conv(void *p);
int main(int argc, char *argv[])
{
int n=3;
void *p = (void *)n;
int m = (int)p;
printf("Value=%d\n", m);
printf("Value=%d\n", conv(p));
if ( createDirectories("/home/projects/abcd/cbd/1/2/3") == -1)
{
		printf("NOT Created:%d\n", errno);
}
else
		printf("Created\n");


	struct dirent *de;
	DIR *dr;
	if ( argc == 1)
	{
		fprintf(stderr,"Usage %s <dir name>\n", argv[0]);
		return 0;
	}

	if ( (dr = opendir(argv[1]) ) == NULL)
	{
			fprintf(stderr,"Unable to open dir: %s, error:%d\n", argv[1], errno);
			exit (1);
	}

	while( (de = readdir(dr)) != NULL)
	{
		printf("FileName: %s\n", de->d_name);
	}
	closedir(dr);
	return 0;
}

int conv( void *p)
{
	return (int)(p);
}



int createDirectories(char * szDestinationDir)
{

	char *pch;
//	char szTmp[256];	
	char szTmpPath[256];
//printf("Path:%s\n", szDestinationDir);
	strcpy(szTmpPath, szDestinationDir);

	pch = strrchr(szTmpPath,'/');

	if(pch == NULL)
		return 0;

	*pch = '\0';
		
	if ( access(szTmpPath, 0) == -1)
		createDirectories(szTmpPath);
	else
	{
		/* create directory */
		if (mkdir(szDestinationDir, 0755) == -1)
		{
			if ( errno != EEXIST)
				return -1;
		}
	}

	if ( access(szDestinationDir, 0) == -1)
	{
		if (mkdir(szDestinationDir, 0755) == -1)
		{
			if ( errno != EEXIST)
				return -1;
		}
	}
	return 0;
}

char *dos2UnixPath(char *pszPath)
{
	char szTmpPath[256];
	char *pch = szTmpPath;

	strcpy(szTmpPath, szPath);
	int iFund = 0;
	while(*pch != '\0')
	{
		if ( *pch == '\\')
		{
			*pch = '/';
			iFound = 1;
		}
		else
		{
			iFound = 1;
		} 
	
	}
}
