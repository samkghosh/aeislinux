/***************************************************************************
* FileName: usermgmt.cpp
* Purpose: manages users connecting to it
*
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "Message.h"
#include "logs3.h"
#include "ssextern.h"

#define MAX_USERS	20

#define MSG_NOK		0
#define MSG_OK		1
#define RET_MAX_REACHED	2


typedef struct
{
	int fd;
}T_User;

static T_User gUserTable[MAX_USERS];

pthread_mutex_t  gCriSec;

int delUser(int fd);
int AddUser(int fd);
int sndWebPkt(int iFd, int iType, char *pszMsg);
int getSiteCount();
int sendAllSiteInfo(int fd);
int getStatus(int idx, char *pszSt);

int sndWebPkt(int iFd, int iType, char *pszMsg)
{
	return 0;
}
/***********************************************************************************************
*
************************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"initUserTable"
int initUserTable()
{
	char szError[256];
	
//	InitializeCriticalSection(&gCriSec);
	//TBD: 2021 check error
	pthread_mutex_init(&gCriSec,NULL);

	memset(&gUserTable, '\0', sizeof(gUserTable));

		return 0;
}


/*************************************************************************************
*
**************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processAddUser"
int processAddUser(int fd, int sktType)
{
	Message *oMsg = new Message(fd);
	char szMsg[32];
	int rc = 0;
	

	McDbgLog(LOGLEVEL3,"Entry");
	try
	{

		rc = AddUser(fd);
		
		sprintf(szMsg,"%d", rc);
		if ( rc < 0 )
		{
			if ( sktType == 0)
				oMsg->sendPkt(szMsg, MSG_NOK, strlen(szMsg)+1);
			else
			{
				sndWebPkt(fd, MSG_NOK, szMsg);
			}
		}
		else
		{
			if ( sktType == 0)
				oMsg->sendPkt(szMsg, MSG_OK, strlen(szMsg)+1);
			else
			{
				sprintf(szMsg,"%d", getSiteCount());
				sndWebPkt(fd, 1, szMsg);

				sendAllSiteInfo(fd);


			}
		}
	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0, "Exception Caught...");
	}
	delete oMsg;
	McDbgLog(LOGLEVEL3,"Exit");

	return rc;
}


/*************************************************************************************
*
**************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processCaptureDeRegisterAgent"
int processDelUser(int fd, int sktType)
{

	Message *oMsg = new Message(fd);
	int rc=0;

	try
	{
		McDbgLog(LOGLEVEL3,"Entry");

		rc = delUser(fd);
	
		if ( sktType == 0)
			oMsg->sendPkt("", MSG_OK, 0);
		else
			sndWebPkt(fd, MSG_OK, "");

	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0, "Exception Caught...");
	}
	delete oMsg;

	McDbgLog(LOGLEVEL3,"Exit");

	return 0;
}


int userMsgmtEnterCriticalSection(pthread_mutex_t *cs)
{
	return pthread_mutex_lock(cs);
}

int userMsgmtLeaveCriticalSection(pthread_mutex_t *cs)
{
	return pthread_mutex_unlock(cs);
}
/****************************************************************************************************
* Add a ScreenCap Agent\
* Return: -1: invalid Input
*		-2: Already Registered
*		-3: reconnected
*		>= 0: new entry
*****************************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"RegisterScreenCapAgent"
int AddUser(int fd)
{
	char szBuf[256];
	char *pch;

	int i;
	char szMsg[512];
	int  iFreeSlot = -1;
	int  iDuplicate = 0;

	
	userMsgmtEnterCriticalSection(&gCriSec);

	for ( i = 0; i < MAX_USERS; i++)
	{
	
		if ( gUserTable[i].fd == 0)
		{
			if (iFreeSlot == -1)
				iFreeSlot = i;
			continue;
		}
	}

	
	if ( iFreeSlot < 0)
	{
		userMsgmtLeaveCriticalSection(&gCriSec);
		return RET_MAX_REACHED; //TBD Max License 
	}

	gUserTable[iFreeSlot].fd = fd;

	userMsgmtLeaveCriticalSection(&gCriSec);
	
	return iFreeSlot;
}


/****************************************************************************************************
* Del a ScreenCap Agent\
*****************************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"delUser"
int delUser(int fd)
{
	int i;
	int iFound = 0;
	userMsgmtEnterCriticalSection(&gCriSec);

	for (i = 0; i < MAX_USERS; i++)
	{
		if ( gUserTable[i].fd == fd)
		{
			gUserTable[i].fd = 0;
			break;
		}
	}

	userMsgmtLeaveCriticalSection(&gCriSec);
	return 0;
}

/***************************************************************************/
int sendAllSiteInfo(int fd)
{
	try
	{
		int iSiteCnt = getSiteCount();
		char szStatus[128];

		for (int i=0; i < iSiteCnt; i++)
		{
			getStatus(i+1, szStatus);
			printf("%d,%s\n", i+1,szStatus);
			sndWebPkt(fd, 2, szStatus);
		}
	}
	catch(...)
	{

	}

	return 0;
}


/******************************************************************************/
/***************************************************************************/
int sendSiteInfo(int fd, int iSiteId)
{

	return 0;
}
/**********************************************************************/
int sendToUsers(int iSiteId, char *pszStatus)
{
	int i;
	int iLocked =0;

	/* lock */
	userMsgmtEnterCriticalSection(&gCriSec);
	iLocked = 1;

	try
	{
		for (i = 0; i < MAX_USERS; i++)
		{
			if ( gUserTable[i].fd != 0)
				sndWebPkt(gUserTable[i].fd, 2, pszStatus);
		}
		userMsgmtLeaveCriticalSection(&gCriSec);
		iLocked = 0;
	}
	catch(...)
	{
		if (iLocked == 1)
			userMsgmtLeaveCriticalSection(&gCriSec);
		
	}
	/* unlock */
	return 0;
}
