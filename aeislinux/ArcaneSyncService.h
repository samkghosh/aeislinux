/*****************************************************************
* FileName: arcanesyncservice.h
*
******************************************************************/
#ifndef	__ARCANESYNCSERVICE_H__
#define __ARCANESYNCSERVICE_H__

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <process.h>
#include "logs.h"
#include "database.h"



typedef struct
{
	char szSiteName[64];
	char szDbServerIP[32];
	char szUserId[32];
	char szPwd[32];
	char szDBName[32];
	char szUploadPath[64];
}T_SiteInfo;

#ifndef MAX_SITES
#define MAX_SITES	1024
#endif


int ProcessSyncInfo(T_SiteInfo *pSiteInfo);

#endif 
