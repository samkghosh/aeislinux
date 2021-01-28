/***************************************************************
** msgRcvr.cpp
**
** Purpose:
**	This shall handle the messaging. This shall use TCP/IP socket to 
**	receive and send message.
**
** Global Procedures Defined:
**
**
** Global Procedures Referenced:
**
**
** Global Data Defined:
**
**
** Global Deta Referenced:
**
**
** Revision History:
**
** [0]	20-07-2012	[SG]	Created
** 
****************************************************************/

//#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
//#include <winsock.h>
#include <errno.h>
#include <sys/stat.h>

#include "Message.h"
//#include "comms.h"
#include "logs3.h"
#include "easy.h"
#include "database.h"
#include "ArcaneSyncService.h"
#include "arcanecommonmsg.h"

extern	Logs		dbgLog;
extern DataBase dbMain;

static int	PortNumber;


void	(*UserSigFunc)(int);

void *initReceiveQ(void * thParam);
void * processRequest(void * tcpFd);
int processRemoteStatusMsg(char *pszMsg);
int processSiteStatusMsg(char *pszMsg);
int processRecordingReportMsg(char *buf);

/* [26-09-2016]	[SG]	*/
int putFile(int);

/* 08-03-2017]	[SG]	*/
int setRemoteStatus(char *pszSiteName, int iStatus, int iChanCount, long lFs);

/* [19-04-2017]	[SG]	time last update was made*/
extern long giLastStatusUpdate;
/* [28-01-2021]	[SG]	porting to Linux */
char * dos2UnixPath(char *pszPath);
/********************************************************************/

#define Sleep(N) sleep(N/1000)
#ifndef MAX_PATH
#define MAX_PATH 255
#endif
/******************************************************************/
#define RET_ERR	-1
#define RET_OK	0
/**********************************************************
** Function: initReceiver
**
**	This shall initialise two thread. One is for handling 
**  Input. There shall be some standard commands available
**  Also, a handle shall be provided to the application
**  handle some message other than the standard messages
**  Also the application shall be given way to send Message
**  to a dialog box.
**
**********************************************************/
#undef __FNNAME__
#define __FNNAME__	"initReceiver"
int initReCeiver(int myPortNum)
{
	// create a server thread


//	signal(SIGABRT, closeAll);
//	signal(SIGFPE,  closeAll);
//	signal(SIGILL,  closeAll);
//	signal(SIGINT,  SIG_IGN);
//	signal(SIGSEGV, closeAll);
//	signal(SIGTERM, closeAll);

	pthread_t thId;

	McDbgLog(LOGLEVEL2," Entry");

	PortNumber = myPortNum;
	
  if( pthread_create(&thId,NULL,initReceiveQ, 0)==-1)
  {
		// thread creation error
		McDbgLog(LOGLEVEL0, "[ERROR] initReceiveQ Thread creation error");
		return 0;
	}

	McDbgLog(LOGLEVEL2," Exit");

	return 1;
}/*int initComms(int myPortNum)*/


/*******************************************************************\
 * function		: 
 * purpose		: 
 * Input		:			
 * Return Values:    
 * Description	:
\*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"initReceiveQ"
void *initReceiveQ(void * thParam)
{

	int					fd;
	char szLog[512];
	/* [16-12-2016]	[SG]	*/
	Tcpip *ptcpServer;

	pthread_t thId;
	
	McDbgLog(LOGLEVEL2, "Entry");

	sprintf(szLog,"Going Listen on port:%d", PortNumber);
	McDbgLog(LOGLEVEL1, szLog);


	while (true )
	{
		// create a tcpip server 
		ptcpServer = new Tcpip(PortNumber);


		if ( ptcpServer->errorFlag == true )
		{
			sprintf(szLog,"Unable to Create Receiver on port:%d, Error=%d", PortNumber, errno);
			McDbgLog(LOGLEVEL0, szLog);
			Sleep(60000);
			
			delete ptcpServer;

			continue;
//			return RET_ERR;

		}
		else
		{
			sprintf(szLog,"Create Receiver on port:%d", PortNumber);
			McDbgLog(LOGLEVEL2, szLog);
			break;
		}
	}
	// wait for ever
	FOREVER
	{
		// on receiving a connection create a thread to service that
		if ( ptcpServer->waitForConnection() == SOCKET_ERROR)
			break;

		fd = ptcpServer->getFd();


    		if( pthread_create(&thId,NULL,processRequest,(void *)fd)==-1)
    		{
			// thread creation error
			McDbgLog(LOGLEVEL0, "[ERROR] processRequest Thread creation error");
			continue;
		}

	}
fprintf(stderr,"Exited............................\n");
	delete ptcpServer;

	McDbgLog(LOGLEVEL2, "Exit");
	
	return NULL;

}/*DWORD WINAPI initReceiveQ(void * thParam)*/


/*******************************************************************\
 * function		: 
 * purpose		: 
 * Input		:			
 * Return Values:    
 * Description	:
\*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processRequest"
void *processRequest(void * tcpFd)
{
	int			fd = (int)tcpFd;
	GenMsg			gm;
	int				type;
	char			errBuf[MAX_PATH];
	int				userIndex = -1;
	int				sz;
	Message			*iMsg;
//	int				iChanNum;
	
	
	errBuf[0]='\0';

	sprintf(errBuf, "[MSG]: Incomming Socket Fd :: %d", fd);
	McDbgLog(LOGLEVEL7, errBuf);

	
	iMsg	= new Message(fd);

	/*set keep alive for the client socket*/
	while (true)
	{

		try
		{
			
			if ((sz = iMsg->recvPkt((char *)&gm, &type) < 0 )	)
			{
			
				if (sz == SOCKET_TIMEOUT)
					continue;
			
				//sprintf(errBuf, "[ERROR] Dead Socket Fd :: [%d] RetCode :: [%d]", fd, sz);
				//McDbgLog(LOGLEVEL0, errBuf);
				break;

			}

		}
		catch(...)
		{
			McDbgLog(LOGLEVEL0, "[ERROR] exception caught");
			continue;
		}

		sprintf(errBuf, "[MSG] Get type as %d and sz == %d", type, sz);
		McDbgLog(LOGLEVEL7, errBuf);
		
		
		//if ( sz == 0 )
		//	continue;

		switch(type)
		{
			case MSG_REMOTE_STATUS:	//1
				processRemoteStatusMsg((char *)gm.body);
				break;
			/* [20-11-2012]	[SG]	*/
			case MSG_SITE_STATUS:
				processSiteStatusMsg((char *)gm.body);
				break;
			/* [20-12-2012]	[SG]	*/
			case MSG_REC_REPORT:
				processRecordingReportMsg((char *)gm.body);
				break;

			/* [26-09-2016]	[SG]	Added code to handle myFTP */
			case MSG_FTP_REQ:
				putFile(fd);
				break;

			default:
				break;

		}/*switch(type)*/

	}/*while (true)*/


	iMsg->closeClient();
	delete iMsg;

	return RET_OK;

}/*DWORD WINAPI processRequest(void * tcpFd)*/

#if 0
/*******************************************************************\
 * function		: 
 * purpose		: 
 * Input		:			
 * Return Values:    
 * Description	:
\*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"closeAll"
void closeAll(int v)
{

	if (UserSigFunc != NULL)
		UserSigFunc;


}/*void closeAll(int v)*/
#endif

/**************************************************************
*
*
*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processRemoteStatusMsg"
int processRemoteStatusMsg(char *pszMsg)
{
	T_RemoteStatus *remoteStat;
	char szQry[512];
	char szMsg[512];
	int i;

	try
	{
		/* [19-04-2017]	[SG]	*/
		giLastStatusUpdate = time(NULL);

		remoteStat = (T_RemoteStatus *)pszMsg;
		
		sprintf(szMsg, "Site=%s, Status=%d", remoteStat->szSiteName, remoteStat->iStatus);
		McDbgLog(LOGLEVEL3, szMsg);
#ifndef SERVICES
printf("Status=%s\n", szMsg);
#endif
		/* Update the status */
		sprintf(szQry,"UPDATE tbl_siteInfo SET i_SiteStatus = '%d', i_ChanCount = '%d', d_StatusDate = NOW() WHERE v_SiteName = '%s'",
									remoteStat->iStatus,
									remoteStat->iChanCount,
									remoteStat->szSiteName);
		
		if ( dbMain.ExecuteQuery(szQry) != DB_SUCCESS)
		{

			sprintf(szMsg, "Unable to execute: %s", szQry);
			McDbgLog(LOGLEVEL0, szMsg);
		} 

		McDbgLog(LOGLEVEL3, szQry);

		/* update channel Status */


		for (i = 0; i < remoteStat->iChanCount; i++)
		{
			sprintf(szQry,"REPLACE INTO tbl_sitechanstatus (v_SiteName, i_ChanNum, i_Status, i_Voltage, v_ExtnNum) VALUES ('%s', '%d', '%d', '%d', '%s')",
									remoteStat->szSiteName,
									remoteStat->chanStat[i].iChanNum,
									remoteStat->chanStat[i].iStatus,
									remoteStat->chanStat[i].iVolt,
									remoteStat->chanStat[i].szExtn);

			if ( dbMain.ExecuteQuery(szQry) != DB_SUCCESS)
			{

				sprintf(szMsg, "Unable to execute: %s", szQry);
				McDbgLog(LOGLEVEL0, szMsg);
			} 


		}

		/* [08-03-2017]	[SG]	add site info into table */
		setRemoteStatus(remoteStat->szSiteName, remoteStat->iStatus,remoteStat->iChanCount,-1);

	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0, "Exception caught...");
	}



	return 0;
}




/**************************************************************
* [20-11-2012]	[SG]
*
*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processSiteStatusMsg"
int processSiteStatusMsg(char *pszMsg)
{
	T_SiteStatus *siteStat;
	char szQry[512];
	char szMsg[512];
//	int i;

	try
	{
		McDbgLog(LOGLEVEL2, "Entry");

		siteStat = (T_SiteStatus *)pszMsg;
		
		sprintf(szMsg, "Site=%s, Space=%ul", siteStat->szSiteName, siteStat->lFreeSpace);
		McDbgLog(LOGLEVEL3, szMsg);
#ifndef SERVICES
printf("Status=%s\n", szMsg);
#endif
		/* Update the status */
		sprintf(szQry,"UPDATE tbl_siteInfo SET i_FreeSpace = %d WHERE v_SiteName = '%s'",
									siteStat->lFreeSpace,
									siteStat->szSiteName);

		if ( dbMain.ExecuteQuery(szQry) != DB_SUCCESS)
		{

			sprintf(szMsg, "Unable to execute: %s", szQry);
			McDbgLog(LOGLEVEL0, szMsg);
		} 

		McDbgLog(LOGLEVEL3, szQry);

		/* [08-03-2017]	[SG]	add site info into table */
		setRemoteStatus(siteStat->szSiteName, -1, -1,siteStat->lFreeSpace);

		/* update channel Status */

	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0, "Exception caught...");
	}

	McDbgLog(LOGLEVEL2, "Exit");

	return 0;
}

/*
*
*/
#undef __FNNAME__
#define __FNNAME__	"createDirectories"
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




/**************************************************************
* [20-11-2012]	[SG]
*
*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processRecordingReportMsg"
int processRecordingReportMsg(char *buf)
{
	T_RecCountReport recRep;
	char szMsg[512];
	int i;
	char szLocation[128];
	char szUpldFile[256];
	char szBuf[512];
	char szUpldFileT[256];
	char szSQL[512];

	int nRecordsFetched;
	_CArray2D recArray;
	

	try
	{
		McDbgLog(LOGLEVEL2, "Entry");

		memcpy(&recRep, buf, sizeof(recRep));

		sprintf(szSQL, "SELECT  v_UploadPath from tbl_siteinfo WHERE v_SiteName = '%s'", recRep.szSiteName);


		if ( dbMain.FetchRecord(szSQL,&recArray, 1,&nRecordsFetched) == DB_SUCCESS)
		{
	
			if ( nRecordsFetched >  0 )
			{

				sprintf(szLocation, "%s", recArray.get(1,1));

				sprintf(szUpldFile, "%s\\transaction", szLocation);
				
				//[28-01-2021]	[SG]
				dos2UnixPath(szUpldFile);


	
				if ( access(szUpldFile, 0 ) == -1)
				{
					// create folder for this
					if ( createDirectories(szUpldFile) == -1)
					{
						sprintf(szMsg, "Site=%s, not able to create directoty=%s", recRep.szSiteName, szUpldFile);
						McDbgLog(LOGLEVEL1, szMsg);
						McDbgLog(LOGLEVEL2, "Exit");

						return -1;
					}
				}

				sprintf(szUpldFile,"%s/%s_%x.upld", szUpldFile, recRep.szSiteName, time(NULL));

				sprintf(szUpldFileT,"%sT", szUpldFile);

				for (i  = 0; i < recRep.iCount; i++)
				{
			
					if ( i == 0 )
					{
						sprintf(szBuf,"[LOCAL]\nCount=%d\nSQL%d=REPLACE INTO tbl_reccount (d_recdate, i_count) VALUES ('%s', %d)\n", 
															recRep.iCount,
															i,
															recRep.report[i].szDate,
															recRep.report[i].iRecCount);
					}
					else
					{
						sprintf(szBuf,"SQL%d=REPLACE INTO tbl_reccount (d_recdate, i_count) VALUES ('%s', %d)\n", 
															i,
															recRep.report[i].szDate,
															recRep.report[i].iRecCount);

					}
					egAppendText(szUpldFileT, szBuf);	

				}

				link(szUpldFileT, szUpldFile);
			}
			else
			{
				sprintf(szBuf,"Unable to get the sitename=%s,[%s]", recRep.szSiteName, szSQL);
				McDbgLog(LOGLEVEL1, szBuf);

			}
		/* update channel Status */
		}
		else
		{
			sprintf(szBuf,"Unable to get the sitename=%s", recRep.szSiteName);
			McDbgLog(LOGLEVEL1, szBuf);
		}
	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0, "Exception caught...");
	}

	McDbgLog(LOGLEVEL2, "Exit");

	return 0;
}

/**********************************************************************
* [28-01-2021]	[SG]	Function convert DOS path to unix path
************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"dos2UnixPath"
char * dos2UnixPath(char *pszPath)
{
	char *pch = pszPath;

	while(*pch != '\0')
	{
		if (*pch == '\\')
			*pch = '/'; 
		pch++;
	}
	return pszPath;
}
