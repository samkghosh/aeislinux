/**************************************************************
* FileName: statusmgmt.cpp
* Purpose: keeps the live updated status in the memory
*
* [History]
*	[00]	08-03-2017]	[SG]	Created
***************************************************************/
#include <stdio.h>
#include <string.h>
#include "database.h"
#include "logs3.h"
#include "ssextern.h"

#define MAX_SITE	500
#ifndef MAX_PATH
#define MAX_PATH	255
#endif
#define sticmp strcasecmp
/*********************************************************************/
typedef struct
{
	int iSiteId;
	char szSiteName[32];
	int iStatus;
	char szStatusDate[32];
	int iChanCount;
	char szLastUploadDate[32];
	float fFS;
	int iTodaysRecCount;
}T_Status;

int giSiteCount;
T_Status gStatus[MAX_SITE];

int sendToUsers(int iSiteId, char *pszStatus);


/********************************************************************/
#undef __FNNAME__
#define __FNNAME__ "initStatus"
int initStatus()
{
	int i;
	
	char szSQL[512];
	int nRecordsFetched;
	char szLog[MAX_PATH];
	

	McDbgLog(LOGLEVEL0, "*****************Starting ******************");
#if 0
	CStringArray arrFields[MAX_SITE];
//TBD:2021
	try
	{
		McDbgLog(LOGLEVEL1,"Going To Read Sites and Last Status");
		/* get the Site information from the database */
		
		sprintf(szSQL, "select i_Id, v_SiteName, i_SiteStatus, d_StatusDate, i_ChanCount, i_FreeSpace, i_Status, d_LastSyncTime from tbl_siteinfo ORDER BY v_SiteName");
		if ( ExecSelectQueryMain(szSQL,arrFields,MAX_SITE,&nRecordsFetched, true) == TRUE)
		{
			giSiteCount = nRecordsFetched;

			for (i =0; i < nRecordsFetched; i++)
			{
				gStatus[i].iSiteId = i+1; //atoi(arrFields[i].GetAt(0));
				strcpy(gStatus[i].szSiteName,arrFields[i].GetAt(1));
				gStatus[i].iStatus = atoi(arrFields[i].GetAt(2));
				strcpy(gStatus[i].szStatusDate, arrFields[i].GetAt(3));
				gStatus[i].iChanCount = atoi(arrFields[i].GetAt(4));
				gStatus[i].fFS = atol(arrFields[i].GetAt(5))/1024.0;
//				gStatus[i].iStatus = atoi(arrFields[i].GetAt(6));
				strcpy(gStatus[i].szLastUploadDate,arrFields[i].GetAt(7));
			}

			for (i = 0; i < nRecordsFetched; i++)
			{	
				arrFields[i].RemoveAll();
			}
		}
	}
	catch(...)
	{
			McDbgLog(LOGLEVEL0,"Exception Caught..");
		
	}
#endif
	
	return 0;																				/* return from function */
}

/****************************************************************************************/
int setRemoteStatus(char *pszSiteName, int iStatus, int iChanCount, long lFs)
{
	time_t curTime;
	struct tm *cTm;
	char szStatus[128];

	for (int i = 0; i < giSiteCount; i++)
	{
		if (strcasecmp(gStatus[i].szSiteName, pszSiteName) == 0)
		{
			if ( iStatus != -1)
			{
				gStatus[i].iStatus = iStatus;
				curTime = time(NULL);
				cTm = localtime(&curTime);
				sprintf(gStatus[i].szStatusDate,"%04u-%02u-%02u %02u:%02u:%02u", cTm->tm_year+1900,
																				cTm->tm_mon+1,
																				cTm->tm_mday,
																				cTm->tm_hour,
																				cTm->tm_min,
																				cTm->tm_sec);
			}
			if ( iChanCount != -1)
				gStatus[i].iChanCount = iChanCount;
			if ( lFs != -1)
				gStatus[i].fFS = lFs/1024.0;

			/* Send message to all users connected */
			sprintf(szStatus,"%d,%s,%d,%s,%s,%5.2f,%d,%d",
						gStatus[i].iSiteId,
						gStatus[i].szSiteName,
						gStatus[i].iStatus,
						gStatus[i].szStatusDate,
						gStatus[i].szLastUploadDate,
						gStatus[i].fFS,
						gStatus[i].iChanCount,
						gStatus[i].iTodaysRecCount);
			sendToUsers(i+1, szStatus);
			break;
		}
	}
	return 0;
}

/*****************************************************************************************/
int setLastFileUpdateState(char *pszSiteName)
{
	time_t curTime;
	struct tm *cTm;


	for (int i = 0; i < giSiteCount; i++)
	{
		if (strcasecmp(gStatus[i].szSiteName, pszSiteName) == 0)
		{
				curTime = time(NULL);
				cTm = localtime(&curTime);
				sprintf(gStatus[i].szLastUploadDate,"%04u-%02u-%02u %02u:%02u:%02u", cTm->tm_year+1900,
																				cTm->tm_mon+1,
																				cTm->tm_mday,
																				cTm->tm_hour,
																				cTm->tm_min,
																				cTm->tm_sec);
			break;
		}
	}

	return 0;
}

/*****************************************************************************************/
int getStatus(int idx, char *pszStatus)
{
	pszStatus[0] = '\0';


	if ( idx == -1)
	{
/**	TBD	for (int i=0; i < giSiteCount; i++)
		{
			sprintf(pszStatus,"%d,%s,%d,%s,%s,%u,%d",
							gStatus[i].iSiteId,
							gStatus[i].szSiteName,
							gStatus[i].iStatus,
							gStatus[i].szStatusDate,
							gStatus[i].szLastUploadDate,
							gStatus[i].lFS,
							gStatus[i].iChanCount);
		}
	*/
	}
	else
	{
		if ( idx > 0 && idx <= giSiteCount)
		{
			sprintf(pszStatus,"%d,%s,%d,%s,%s,%5.2f,%d,%d",
						gStatus[idx-1].iSiteId,
						gStatus[idx-1].szSiteName,
						gStatus[idx-1].iStatus,
						gStatus[idx-1].szStatusDate,
						gStatus[idx-1].szLastUploadDate,
						gStatus[idx-1].fFS,
						gStatus[idx-1].iChanCount,
						gStatus[idx-1].iTodaysRecCount);
		}		
	}
	return 0;
}


/***************************************************/
int getSiteCount()
{
	return giSiteCount;
}

/*********************************************************************/
int setRecCount(char *pszSiteName, int iCount)
{
	char szStatus[256];


	for (int i = 0; i < giSiteCount; i++)
	{
		if (strcasecmp(gStatus[i].szSiteName, pszSiteName) == 0)
		{
			gStatus[i].iTodaysRecCount = iCount;
			/* Send message to all users connected */
			sprintf(szStatus,"%d,%s,%d,%s,%s,%5.2f,%d,%d",
						gStatus[i].iSiteId,
						gStatus[i].szSiteName,
						gStatus[i].iStatus,
						gStatus[i].szStatusDate,
						gStatus[i].szLastUploadDate,
						gStatus[i].fFS,
						gStatus[i].iChanCount,
						gStatus[i].iTodaysRecCount);
			sendToUsers(i+1, szStatus);
			break;
		}
	}
	return 0;
}
