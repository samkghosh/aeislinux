/***************************************************************************************************
* FileName: iniFunc.cpp
*
* Purpose:	This file contains the functions that are required to read, write and 
*			retrieve info from a ini file.
*
* Copyright (C) 2015, Alliance Infotech Pvt Ltd, New Delhi
*
* History
*	[00]	[21-01-2015]	Created
****************************************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logs3.h"
#include "ssextern.h"

#include "iniFunc.h"
/* getpvt.cpp : Defines the entry point for the console application.
*/
#ifndef MAX_PATH
#define MAX_PATH 255
#endif
#define TRUE	true
#define FALSE	false

bool bReadINIEntryint(char *szSectName, char *szEntryName, int *piRet, int iDef, char *szName);

/**************************************/


int nGetPrivateProfileString(char *pszSec, char *pszKey, char *pszDef, char *pszRet, int iSize, char *pszFileName)
{
	FILE *fp;
	char szBuf[1024];
	int iSecFound = 0;
	int iKeyFound = 0;
	int iRetSz = 0;
	char szSec[64];
	char *pch;
	char *pch1;
	int i;


//fprintf(stderr,"CFG File=%s\n", pszFileName);
	fp = fopen(pszFileName, "r+t");

	if ( fp == NULL)
       {
//fprintf(stderr,"Unable to open %s, Error=%d\n", pszFileName, errno);
		return -1;
       } 

	sprintf(szSec,"[%s]", pszSec);

	while(1)
	{
		if ( feof(fp))
			break;

		if ( fgets(szBuf, sizeof(szBuf), fp) == NULL)
			break;
//fprintf(stderr,"%s\n", szBuf);
		pch = strrchr(szBuf,'\r');
		if (pch != NULL)
			*pch = '\0';

		pch = strrchr(szBuf,'\n');
		if (pch != NULL)
			*pch = '\0';

		if ( szBuf[0] == '/' || szBuf[0] == '#' || szBuf[0] == '\\')
			continue;

//	fprintf(stderr,"Buf=%s, Sec=%s, key=%s\n", szBuf, szSec, pszKey);	

		if ((i= strcasecmp(szBuf, szSec)) == 0)
		{
			iSecFound = 1;
			continue;
		}

		if ( szBuf[0] == '[' && iSecFound == 1 )
		{
			/* New Section */
			break;
		}

		if ( iSecFound == 1 )
		{
			pch = strtok(szBuf, " =\t");

			if ( pch == NULL)
				continue;

			if ( strcasecmp(szBuf, pszKey) == 0 )
			{
				iKeyFound = 1;
				pch = strtok(NULL, "=\r\n");

				if ( pch == NULL)
				{
					pszRet[0] = '\0';
					iRetSz = 0;
					break;
				}
				
				pch1 = pch;

				while (*pch1 != '\0' && *pch1 == ' ')
					pch1++;
				strncpy(pszRet, pch1, iSize);
				iRetSz = strlen(pszRet);
				break;
			}
			else
				continue;
		}

	}

	fclose(fp);

	if ( iSecFound == 0)
		return -1;

	if ( iKeyFound == 0 )
	{
		strcpy(pszRet, pszDef);
		iRetSz = strlen(pszRet);
	}
//fprintf(stderr,"Sie=%d\n", iRetSz);
	return iRetSz;

}




int nGetPrivateProfileInt(char *pszSec, char *pszKey, int iDef, char *pszFileName)
{
	FILE *fp;
	char szBuf[1024];
	int iSecFound = 0;
	int iKeyFound = 0;
	int iRetSz = 0;
	char szSec[64];
	char *pch;
	char *pch1;
	int i;



	fp = fopen(pszFileName, "r+t");

	if ( fp == NULL)
		return -1;

	sprintf(szSec,"[%s]", pszSec);


	while(1)
	{
		if ( feof(fp))
			break;

		if ( fgets(szBuf, sizeof(szBuf), fp) == NULL)
			break;

		pch = strrchr(szBuf,'\r');
		if (pch != NULL)
			*pch = '\0';

		pch = strrchr(szBuf,'\n');
		if (pch != NULL)
			*pch = '\0';

		if ( szBuf[0] == '/' || szBuf[0] == '#' || szBuf[0] == '\\')
			continue;
		
		if ((i= strcasecmp(szBuf, szSec)) == 0)
		{
			iSecFound = 1;
			continue;
		}

		if ( szBuf[0] == '[' && iSecFound == 1 )
		{
			/* New Section */
			break;
		}

		if ( iSecFound == 1 )
		{
			pch = strtok(szBuf, " =\t");

			if ( pch == NULL)
				continue;

			if ( strcasecmp(szBuf, pszKey) == 0 )
			{
				iKeyFound = 1;
				pch = strtok(NULL, "=\r\n");

				if ( pch == NULL)
				{
					iRetSz = 0;
					break;
				}
				
				pch1 = pch;

				while (*pch1 != '\0' && *pch1 == ' ')
					pch1++;

				iRetSz = atol(pch1);
				
				break;
			}
			else
				continue;
		}

	}

	fclose(fp);

	if ( iSecFound == 0)
		return -1;

	if ( iKeyFound == 0 )
	{
		iRetSz = iDef;
	}

	return iRetSz;

}

/***************************************************************************************************
* Proc Name: readIni
* Input:
*	void
*Output
*	int 
*Processing:
*	Read data from ini files
*
*	
***************************************************************************************************/
#undef		__FNNAME__
#define		__FNNAME__  "ReadIni"
int readIni()
{
	char	szLog[MAX_PATH] ;
	char	szLogPath[MAX_PATH] ;
	int		iLogLevel;
	char	szStorageFormat[256];
	char    cCurrentPath[256];
	int iTmpVal;
	char szIniFileName[256];
	char szKey[32];
	int i;

	sprintf(szIniFileName,"%s", gszINIFileName);
	
	//read log path from ini file.
	if(bReadINIEntry("PATH","basepath", gszBasePath, "", sizeof(gszBasePath),szIniFileName) == false)
  	{
//fprintf(stderr,"Basepath entry not found.\n");
       		return 0;         
  	}
	else
	{
		//fprintf(stderr,"Basepath=%s.\n", gszBasePath);
	}
        sprintf(szLogPath,"/var/log/arcane/");

	//read log path from ini file.
	if (bReadINIEntryint("main", "loglevel", &iLogLevel, 0xff,szIniFileName)== false)//Read Logfilepath
	{
	   iLogLevel	=	0x7;		//set log level to 3.		
	}
	giLogLevel = iLogLevel;	
	// Now initialise the Log Lib
	dbgLog.initLogs(szLogPath,"SYN", 1, iLogLevel);
	sprintf(szLog,"*********** Startup: Compiled on %s, %s***********", __DATE__, __TIME__);
	McDbgLog(LOGLEVEL0, szLog);
//fprintf(stderr,"[%s]%s\n", szLogPath, szLog);
	// Read voice recording file path

	
	if( bReadINIEntry("Database", "DBIPAddress", gszDBIPAddress, "", sizeof(gszDBIPAddress), szIniFileName)== false )	//Read DSN
		strcpy(gszDBIPAddress,"127.0.0.1");

		
	bReadINIEntry("database", "dbuid", gszUID, "root", sizeof(gszUID), szIniFileName);
	
	
	bReadINIEntry("database", "dbpwd", gszPWD, "", sizeof(gszPWD), szIniFileName);	//Read PWD

	
	bReadINIEntry("database", "dbname", gszDBName, "", sizeof(gszDBName), szIniFileName);	//Read PWD


	bReadINIEntryint("main","port",&giPortNum,0, szIniFileName);
	
	sprintf(szLog,"Listening Port=%d", giPortNum);
	McDbgLog(LOGLEVEL2, szLog);

	bReadINIEntryint("main","count",&giNumOfSites,0, szIniFileName);
	sprintf(szLog,"Number of Sites=%d", giNumOfSites);
	McDbgLog(LOGLEVEL2, szLog);

	bReadINIEntryint("main","statusport",&giNumOfSites,5678, szIniFileName);


	bReadINIEntry("PATH", "ftppath", gszFTPPath,gszBasePath, sizeof(gszFTPPath), szIniFileName);

//fprintf(stderr,"End of INI\n");
	return 0;
}



/********************************************************************************************
* Proc Name : bool bReadINIEntry (char *szSectName, 
*									char *szEntryName, char *szRet, char *szDef, int iSize, char *szName)
*
* Purpose:
* Reads an INI entry from the specified INI file
*
* Algorithms:
* none
*
* Parameters:
* szSectName		= INI section name
* szEntryName		= INI entry name
* szDef				= default value
* szName			= name of the ini file
* szRet(out)		= The retrieved value
*
* Retval:
* true :	if successful
* false:	if failed
*
**********************************************************************************************/
#undef __FNNAME__
#define __FNNAME__ "bReadINIEntry"
bool bReadINIEntry(char *szSectName, char *szEntryName, char *szRet, char *szDef, int iSize, char *szName)
{
	bool	bRet;
//fprintf(stderr,"Sec=%s, key=%s\n", szSectName, szEntryName);

        if(nGetPrivateProfileString(szSectName,szEntryName,szDef,szRet,iSize,szName)==-1)
	{
//fprintf(stderr,"Unable to Read INI values: Sec:%s, Key=%s, Def=%s, ret=%s, iSize=%d, FileName=%s\n", szSectName, szEntryName, szDef, szRet, iSize, szName);
           //WritePrivateProfileString(szSectName, szEntryName, szDef, szName);
		bRet = false;
	}
	else
	{
		//if(szRet[0] != 0)		//if we have not read anything.
		//	WritePrivateProfileString(szSectName, szEntryName, szRet, szName);
//fprintf(stderr,"Read INI values: Sec:%s, Key=%s, Def=%s, ret=%s, iSize=%d, FileName=%s\n", szSectName, szEntryName, szDef, szRet, iSize, szName);
		bRet = true;
	}

//fprintf(stderr,"Ret Val=%d\n", bRet);
	return bRet;
}




/********************************************************************************************
* Proc Name : bool bReadINIEntryint (char *szSectName, 
*									char *szEntryName, char *szRet, char *szDef, char *szName)
*
* Purpose:
* Reads an integer INI entry from the specified INI file
*
* Algorithms:
* none
*
* Parameters:
* szSectName		= INI section name
* szEntryName		= INI entry name
* szDef				= default value
* szName			= name of the ini file
* szRet(out)		= The retrieved value
*
* Retval:
* true :	if successful
* false:	if failed (if no entry exists, entry is not an interger)
*
**********************************************************************************************/
#undef __FNNAME__
#define __FNNAME__ "bReadINIEntryint"
bool bReadINIEntryint(char *szSectName, char *szEntryName, int *piRet, int iDef, char *szName)
{
	char		szRet[MAX_PATH];		//Array will contain the Output string read from ini file.
	char		szDef[MAX_PATH];
	bool		bRet = true;

	sprintf(szDef, "%d", iDef);
	szRet[0] = '\0';

	if((nGetPrivateProfileString(szSectName, szEntryName, szDef, szRet, MAX_PATH, szName)) == false)
	{
		//itePrivateProfileString(szSectName, szEntryName, szDef, szName);
		//
		*piRet = iDef;

	}
	else
	{
	//if(szRet[0] != '0')
		//WritePrivateProfileString(szSectName, szEntryName, szRet, szName);
		if ( szRet[0] == '\0')
			*piRet = iDef;
		else
			*piRet = atoi(szRet);
	}
//fprintf(stderr,"Read INI values: Sec:%s, Key=%s, Def=%d,[%s] ret=%d, FileName=%s\n", szSectName, szEntryName, iDef, szRet,  *piRet, szName);
	return bRet;
}






/********************************************************************
 * [01-07-2015]	[SG]
 * Periodic update from the INI
 * *******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"periodicINIUpdate"
void *periodicINIUpdate(void *p)
{
	char	szLog[MAX_PATH] ;
	int		iLogLevel;
	char	szStorageFormat[256];
	char    cCurrentPath[256];
	int iTmpVal;
	char szIniFileName[256];
	char szKey[32];
	int i;

	sprintf(szIniFileName,"%s", gszINIFileName);
	
	sleep(300);

	while( true )
	{	
		try
		{
			//read log path from ini file.
			if (bReadINIEntryint("main", "loglevel", &iLogLevel, 0xff,szIniFileName)== false)//Read Logfilepath
			{
	 		  iLogLevel	=	0x7;		//set log level to 3.		
			}
			giLogLevel = iLogLevel;	


		}
		catch(...)
		{
			sleep(10);
		}
		sleep(20);
	}
	return NULL;
}

#if 0
bool readIni()
{
	
	/* ReadLog dir path */
	if((nGetPrivateProfileString("Main","LogDir","",gszLogDir,sizeof(gszLogDir),gszINIFileName)) <= 0)
		return FALSE;

		/* Read Log level */
	giLogLevel = nGetPrivateProfileInt("Main","LogLevel",255,gszINIFileName);

	giNumOfSites= nGetPrivateProfileInt("Main","COUNT",0, gszINIFileName);

	giPortNum = nGetPrivateProfileInt("Main","StatusPort",5678, gszINIFileName);

	/* [27-09-2016]	[SG]	*/
	if (nGetPrivateProfileString("MAIN", "FTPPath", "", gszFTPPath, sizeof(gszFTPPath), gszINIFileName) <= 0)
		strcpy(gszFTPPath,gszBasePath);

	return			TRUE;

}/* End of readIniSettings function */
#endif

/************************** end of file ****************************************/
