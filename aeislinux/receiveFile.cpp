/****************************************************************
*
* FileName:receiveFile.cpp
*
* Process: 
*
* Application:	
*
* Copyright (C), 2014, Alliance-Infotech (P) Ltd.
*
* Purpose: Receives files from the client
*
*operations.
*
* Notes:
* * NONE *
*
* Global routines:
*	
*
*
*
* Local routines: 
*
*
*
*
*
* Revision History:
*	[00]	[01-11-2014]	[SG]	Created
*
**************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <time.h>
#include "Message.h"
#include "arcanecommonmsg.h"
#include "logs3.h"
#include "easy.h"
#include "iniFunc.h"

#include "ssextern.h"

extern long giLastStatusUpdate;

static int giMaxCap;

extern Logs dbgLog;
int calcCRCofFile(char *pszFileName);
int createDirectories(char * szDestinationDir);
/* [28-01-2021]	[SG]	Added */
char *dos2UnixPath(char *pszPath);

/*******************************************************************/
#ifndef MAX_PATH
#define MAX_PATH	255
#endif

/*******************************************************************\
 * function		: getCapturedFile
 * purpose		: Gets the captured file over IP
 * Input		:			
 * Return Values:    
 * Description	:
\*******************************************************************/
#undef __FNNAME__
#define	__FNNAME__	"putFile"
int putFile(int fd) //char *pszSourceFileName, char *pszDestPath)
{
//	char szMsg[256];
	Tcpip		*tcpc;
	Message		*oMsg;
	Message		*iMsg;
	GenMsg		msg;
	int			iCaptureError = 0;
	FILE		*fp = NULL;
	char		szErrMsg[512];
//	T_FileBuffer	*fileBuf;
	int			iCurrSize=0;
	int			iTotalSize=0;
	int			type;
	char		szFileName[MAX_PATH];
	int			iStartTime=time(NULL);
	int			iNo=1;
	int iCRC;
	char szDestFileName[256];
	T_FtpHeader *ftpH;
	char szSourceFile[128];
	char szDestPath[128];
	char *pch;
	int iSz = 0;

	/* [19-04-2017]	[SG]	Fix file size to 6 MB */
	int iMaxFileSize= 6 * 1024 * 1024;
	
	/* [11-12-2018]	[SG]	*/
	int iMaxFileSizeinMB = nGetPrivateProfileInt("Main","TxSz",6, gszINIFileName);

	iMaxFileSize = iMaxFileSizeinMB * 1024 *1024;

	oMsg = new Message(fd);
	iMsg = new Message(fd);

	/* Send Request to get the file */

	oMsg->sendPkt("", MSG_FTP_START,0);

	/* [28-09-2016]	[SG]	Added Timeout */
	setRcvTimeOut(fd, 30000);

	while ((iSz = iMsg->recvPkt((char *)&msg, &type)) > 0)
	{
		/* [16-09-2017]	[SG]	*/
		giLastStatusUpdate = time(NULL);

//sprintf(szErrMsg,"RcvType=%d", type);
//McDbgLog(LOGLEVEL3, szErrMsg);
		switch(type)
		{
			case MSG_RCV_FILE_START:

				
				iStartTime = time(NULL);
				// create the file
			//	sprintf(szFileName, "%s\\%s", gszVideoFilePath, gScreenCapTable[iChanNum-1].szCaptureFileName);
				ftpH = (T_FtpHeader *)msg.body;
				strcpy(szSourceFile,ftpH->szSource);
				
				sprintf(szDestPath,"%s/%s", gszFTPPath,ftpH->szDest);
				
				iTotalSize=ftpH->iSize;

				pch = szDestPath;
#if 0
//This path for windows
				while(*pch != '\0')
				{
					if ( *pch == '/')
						*pch = '\\';
					pch++;
				}
				sprintf(szDestFileName, "%s\\%s", szDestPath, szSourceFile);
#else
// for Unix 
				dos2UnixPath(szDestPath);
				sprintf(szDestFileName, "%s/%s", szDestPath, szSourceFile);


#endif
				
				if ( access(szDestPath, 0) == -1)
				{
					sprintf(szErrMsg,"DestPath=%s Not present, will create.",szDestPath);
					McDbgLog(LOGLEVEL2,szErrMsg);

					if (createDirectories(szDestPath) == -1)
					{
						sprintf(szErrMsg,"Unable to create DestPath=%s. File Copy Failed.",szDestPath);
						McDbgLog(LOGLEVEL1,szErrMsg);
		
						oMsg->sendPkt("", MSG_FILE_TX_REMOTE_ERROR,0);
						break;

					}
				}
				
			//	if ( (fp = fopen(szFileName,"wb")) == NULL)
				if ( (fp = fopen(szDestFileName,"wb")) == NULL)
				{
					sprintf(szErrMsg,"Unable to create file [%s], errno=%d", szDestFileName, errno);
					McDbgLog(LOGLEVEL0, szErrMsg);
					oMsg->sendPkt("", MSG_FILE_TX_REMOTE_ERROR,0);
					break;
				}
				else
				{
					sprintf(szErrMsg,"File=%s is Opened Successfully, Sending to get next buffer", szDestFileName);
					McDbgLog(LOGLEVEL2, szErrMsg);
					oMsg->sendPkt("", MSG_GET_NEXT,0);
				}
				break;

			case MSG_RCV_FILE:
				//fileBuf = (T_FileBuffer *)&msg.body;

				//fwrite(fileBuf->data, fileBuf->size, 1, fp);
				fwrite((char *)msg.body, (iSz-4), 1, fp);
				iCurrSize += (iSz-4);
#ifdef CONSOLEMODE
				printf("Rcvd=%d, Total=%d\n", iSz-4, iCurrSize);
#endif
				sprintf(szErrMsg,"Rcvd=%d, Total=%d, File=%s", (iSz-4), iCurrSize, szDestPath);
				McDbgLog(LOGLEVEL3, szErrMsg);
				/*				if ( (iCurrSize / (1024* 100 * iNo )) > 0 )
				{
					iNo++;
					oMsg->sendPkt("", MSG_GET_NEXT,0);
				}
*/
				if ( iCurrSize >= iMaxFileSize)
				{
					fclose(fp);
					fp = NULL;
					oMsg->sendPkt("", 
							MSG_FILE_TX_REMOTE_ERROR,
							0);

					
					sprintf(szErrMsg,"FileSizeof=%s is %d, rejecting it.", szDestPath, iCurrSize);
					McDbgLog(LOGLEVEL1, szErrMsg);
					iCaptureError = 1;

					break;

				}
				iNo++;
				oMsg->sendPkt("", MSG_GET_NEXT,0);

				break;

			case MSG_RCV_END_FILE:
				sprintf(szErrMsg,"End of file rcvd for %s", szDestFileName);
				McDbgLog(LOGLEVEL2, szErrMsg);

				fclose(fp);
				fp = NULL;


				if ( (iCRC = calcCRCofFile(szDestFileName)) == -1)
				{
					sprintf(szErrMsg,"Unable to get CRC for %s, File Not Copied properly.", szDestFileName);
					McDbgLog(LOGLEVEL0, szErrMsg);

					oMsg->sendPkt("", 
							MSG_FILE_CRC_ERROR,
							0);
				}
				else
				{
					sprintf(szErrMsg,"CRC of %s is [%d][%x].", szDestFileName, iCRC, iCRC);
					McDbgLog(LOGLEVEL2, szErrMsg);

					sprintf(szErrMsg,"%d", iCRC);
					oMsg->sendPkt((char *)szErrMsg, MSG_FILE_CRC, strlen(szErrMsg)+1);
				}
				break;

			case MSG_FILE_SENT_OK:
					sprintf(szErrMsg,"File Rcvd %s is OK.", szDestFileName);
					McDbgLog(LOGLEVEL2, szErrMsg);
				break;

			case MSG_FILE_SENT_NOK:
					sprintf(szErrMsg,"File Rcvd %s is NOT OK.", szDestFileName);
					McDbgLog(LOGLEVEL0, szErrMsg);
				unlink(szDestFileName);
				break;

			case MSG_RCV_NO_FILE:
				iCaptureError = 1;
				sprintf(szErrMsg,"Requested File not found [%s]", 
							szDestFileName);
				McDbgLog(LOGLEVEL0, szErrMsg);

				break;

		}

		if ( iCaptureError == 1 )
			break;
	}

	if ( fp != NULL)
		fclose(fp);
	
	if ( iCaptureError == 1)
	{
		sprintf(szErrMsg,"Error in getting file %s, Size rcvd=%d, TotalSize=%d", 
							szSourceFile,
							iCurrSize, iTotalSize);
		McDbgLog(LOGLEVEL0, szErrMsg);

		return -1;
	}
	else
	{
		sprintf(szErrMsg,"File Rcvd Successfullyin [%s], Size rcvd=%d, TotalSize=%d, Time=%d Sec. No Of Frames=%d", 
							szDestFileName,
							iCurrSize, iTotalSize, time(NULL) -iStartTime , iNo);

		McDbgLog(LOGLEVEL2, szErrMsg);
	}


	return 0;

}


/*******************************************************
*
**********************************************************/
int calcCRCofFile(char *pszFileName)
{
	FILE *fp;
	struct	stat		statBuf;
	char *pszBuf;
	int CRC;


	egInitcrc();


	if ( (fp=fopen(pszFileName,"r+b")) == NULL)
		return -1;
	stat(pszFileName,&statBuf);

	pszBuf = (char *)malloc(statBuf.st_size);
	
	if ( fread(pszBuf, statBuf.st_size,1, fp) == 0)
	{
		fclose(fp);
		free(pszBuf);
		return -1;
	}
	fclose(fp);

	CRC = egIcrc(0, (unsigned char *)pszBuf, statBuf.st_size, (short)0, -1);
	free(pszBuf);
	return CRC;
}
