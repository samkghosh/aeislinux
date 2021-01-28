/***************************************************************
** webcom.cpp
**
** Purpose:
**	This shall handle the messaging between web socket and C++. This shall use TCP/IP socket to 
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
** [00]	[04-05-2016] [SG]	Created
**
****************************************************************/


/*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "Message.h"
//#include "comms.h"
#include "logs.h"
#include "easy.h"
#include "sha1.h"

#define RET_ERR		-1
#define RET_OK		0
#define MSG_CAPTURE_REGISTER_AGENT 320
#define MSG_CAPTURE_DEREGISTER_AGENT	321



extern					Logs		dbgLog;

int Encode64(char *pszInBuf, int inSz, char *pszOut);
int Decode64(char *source, unsigned char *target, size_t targetlen);

static int				PortNumber;
/* [05-02-2013]	[SG]	*/
extern int sendDS(int iChanNum, char *pszBuf, int iSize, int iFormat);

int checkAgent(char *pszUid, char *szMsg);
DWORD WINAPI initWebReceiveQ(void * thParam);

int getTypeNMsg(char *pszMsgBuf, char *pszMsg);

/*[02-04-2016]	[SG]	*/
/*int processCaptureRegisterAgent(int fd, char *body);
int processCaptureDeRegisterAgent(int fd, char *body);
int processCaptureStart(int fd, char *body);
int processCaptureStop(int fd, char *body);
int processCaptureMultpartStart(int fd, char *body);
int processCaptureEndMsg(int fd, char *pszMsg);
*/
int processDelUser(int fd, int sktType);
int processAddUser(int fd, int sktType);
int delUser(int fd);

/**********************************************************
** Function: initComms
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
#define __FNNAME__	"initWebComms"
int initWebComms(int myPortNum)
{
	// create a server thread

	HANDLE thHandle;
	DWORD	dwThId;

/*	signal(SIGABRT, closeAll);
	signal(SIGFPE,  closeAll);
	signal(SIGILL,  closeAll);
	signal(SIGINT,  SIG_IGN);
	signal(SIGSEGV, closeAll);
	signal(SIGTERM, closeAll);
*/
	PortNumber = myPortNum;

	thHandle = CreateThread( 
				            NULL,          // no security attributes 
				            0,             // use default stack size  
                            initWebReceiveQ,  // thread function 
                            NULL,	       //&dwThrdParam, // argument to thread function 
                            0,             // use default creation flags 
                            &dwThId);      // returns the thread identifier 
 
	if (thHandle == NULL )
	{
		// thread creation error
		McDbgLog(LOGLEVEL0, "[ERROR] initReceiveQ Thread creation error");
		return RET_ERR;
	}
	CloseHandle(thHandle);

	return RET_OK;


}/*int initWebComms(int myPortNum)*/


/*
*
*/
bool ComputeHash(char *pszKey, char *pszHashBytes)
{
	CSHA1 sh1;
	UINT_8 x[256];
	char h[50]="";
	char h1[4];

	sh1.Reset();
	sh1.Update((UINT_8 *)pszKey,strlen(pszKey));

	sh1.Final();
	bool b = sh1.GetHash(x);

	if (b == true)
	{
/*		for (int i = 0; i < 20; i++)
		{
			
			sprintf(h1,"%02x", x[i]);
			strcat(h,h1);
		}
*/
		Encode64((char *)x, 20, pszHashBytes);
		return true;
	}
	else
		return false;
}


byte *GetEncodedData(char *pszMsg, int iLen)
{
//            byte[] rawData = System.Text.Encoding.ASCII.GetBytes(msg);

            int frameCount = 0;
            unsigned char frame[10];
			int i;


            frame[0] = (byte)129;

            if (iLen <= 125)
            {
                frame[1] = iLen;
                frameCount = 2;
            }
            else if (iLen >= 126 && iLen <= 65535)
            {
                frame[1] = (byte)126;
                int len = iLen ;
                frame[2] = (byte)((len >> 8) & (byte)255);
                frame[3] = (byte)(len & (byte)255);
                frameCount = 4;
            }
            else
            {
                frame[1] = (byte)127;
                int len = iLen;
                frame[2] = (byte)((len >> 56) & (byte)255);
                frame[3] = (byte)((len >> 48) & (byte)255);
                frame[4] = (byte)((len >> 40) & (byte)255);
                frame[5] = (byte)((len >> 32) & (byte)255);
                frame[6] = (byte)((len >> 24) & (byte)255);
                frame[7] = (byte)((len >> 16) & (byte)255);
                frame[8] = (byte)((len >> 8) & (byte)255);
                frame[9] = (byte)(len & (byte)255);
                frameCount = 10;
            }

            int iFLength = frameCount + iLen;

            byte *reply = new byte[iFLength+1];

            int bLim = 0;
            for (i = 0; i < frameCount; i++)
            {
                reply[bLim] = frame[i];
                bLim++;
            }
            for (i = 0; i < iLen; i++)
            {
                reply[bLim] = pszMsg[i];
                bLim++;
            }
			reply[bLim] = '\0';
            return reply;
}


/*********************************************************
*
**********************************************************/
int GetDataLen(char *buffer)
{
    byte b = buffer[1];
    int dataLength = 0;
    int totalLength = 0;
	byte szLengthInfo[16];
	union x
	{
		short i;
		struct y
		{
			byte a;
			byte b;
		}y;
	}xx;

    if (b - 128 <= 125)
    {
        dataLength = b - 128;
        totalLength = dataLength + 6;
    }
    if (b - 128 == 126)
    {
//        dataLength = BitConverter.ToInt16(new byte[] { buffer[3], buffer[2] }, 0);
		xx.y.a = (byte)buffer[3];
		xx.y.b = (byte)buffer[2];
		dataLength = xx.i;
        totalLength = dataLength + 8;
    }

    if (b - 128 == 127)
    {
//        dataLength = (int)BitConverter.ToInt64(new byte[] { buffer[9], buffer[8], buffer[7], buffer[6], buffer[5], buffer[4], buffer[3], buffer[2] }, 0);
		szLengthInfo[0] = buffer[9];
		szLengthInfo[1] = buffer[8];
		szLengthInfo[2] = buffer[7];
		szLengthInfo[3] = buffer[6];
		szLengthInfo[4] = buffer[5];
		szLengthInfo[5] = buffer[4];
		szLengthInfo[6] = buffer[3];
		szLengthInfo[7] = buffer[2];
		szLengthInfo[8] = '\0';
		dataLength = atol((char *)szLengthInfo);
        totalLength = dataLength + 14;
    }

	return totalLength;
}


/******************************************************************************
* Function to decode the received frame and get the message out
********************************************************************************/
char * GetDecodedData(char *buffer, int length)
{
    byte b = buffer[1];
    int dataLength = 0;
    int totalLength = 0;
    int keyIndex = 0;
	byte szLengthInfo[9];

	//b = ntohs(b);

    if (b - 128 <= 125)
    {
        dataLength = b - 128;
        keyIndex = 2;
        totalLength = dataLength + 6;
    }

    if (b - 128 == 126)
    {
		dataLength = GetDataLen(buffer) -8 ; //atoi((char *)szLengthInfo);

		keyIndex = 4;
        totalLength = dataLength + 8;
    }

    if (b - 128 == 127)
    {
		dataLength = GetDataLen(buffer); //atoi((char *)szLengthInfo);
        keyIndex = 10;
        totalLength = dataLength + 14;
    }

    if (totalLength > length)
		return NULL;
//        throw new Exception("The buffer length is small than the data length");

//    char byte[] key = new byte[] { buffer[keyIndex], buffer[keyIndex + 1], buffer[keyIndex + 2], buffer[keyIndex + 3] };
    byte key[8] = { buffer[keyIndex], buffer[keyIndex + 1], buffer[keyIndex + 2], buffer[keyIndex + 3] };
	key[4] = '\0';

    int dataIndex = keyIndex + 4;
    int count = 0;
    for (int i = dataIndex; i < totalLength; i++)
    {
        buffer[i] = (byte)(buffer[i] ^ key[count % 4]);
        count++;
    }
	memcpy(buffer, buffer+dataIndex, dataLength);
	buffer[dataLength] = '\0';
    //return System.Text.Encoding.ASCII.GetString(buffer, dataIndex, dataLength);
	return buffer;
}


DWORD WINAPI processWebRequest(void * tcpFd);

/*******************************************************************\
 * function		: 
 * purpose		: 
 * Input		:			
 * Return Values:    
 * Description	:
\*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"initWebReceiveQ"
DWORD WINAPI initWebReceiveQ(void * thParam)
{

	HANDLE				thHandle;
	DWORD				dwThId;
	int					fd;
	char	szLogMsg[512];
	Tcpip				* tcpServer; //(PortNumber);

	try
	{
		// create a tcpip server 
		//Tcpip				tcpServer(PortNumber);
	TRYAGAIN:

		 tcpServer = new Tcpip(PortNumber);

		if ( tcpServer->errorFlag == true )
		{
			sprintf(szLogMsg,"Unable to Initialise Receiver.Error Code=%d", GetLastError());
			McDbgLog(LOGLEVEL0, szLogMsg);
			delete tcpServer;
			Sleep(60000);
			goto TRYAGAIN;
			//return RET_ERR;
		}

		// wait for ever
		FOREVER
		{
			// on receiving a connection create a thread to service that
			if ( tcpServer->waitForConnection() == SOCKET_ERROR)
			{
				sprintf(szLogMsg,"Error in waiting for Connection. Error Code=%d", GetLastError());
				McDbgLog(LOGLEVEL0, szLogMsg);
				delete tcpServer;

				Sleep(15000);
				goto TRYAGAIN;
			}
			fd = tcpServer->getFd();


			thHandle = CreateThread( 
									 NULL,            // no security attributes 
									 0,               // use default stack size  
									 processWebRequest,  // thread function 
									 (void *)fd,	  // argument to thread function 
									 0,               // use default creation flags 
									 &dwThId);        // returns the thread identifier 
	 
			if (thHandle == NULL )
			{
				// thread creation error
				McDbgLog(LOGLEVEL0, "[ERROR] processRequest Thread creation error");
				return RET_ERR;
			}

				CloseHandle(thHandle);
			}
	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0, "[Exception caught..");
	}
	return RET_OK;

}/*DWORD WINAPI initReceiveQ(void * thParam)*/


/*******************************************************************\
 * function		: 
 * purpose		: 
 * Input		:			
 * Return Values:    
 * Description	:
\*******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"processWebRequest"
DWORD WINAPI processWebRequest(void * tcpFd)
{
	char *pszGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	int				fd = (int)tcpFd;
	GenMsg			gm;
	int				type;
	char			errBuf[MAX_PATH];
	int				userIndex = -1;
	int				iAgentIndex = -1;
	int				sz;
	Message			*iMsg;
	char szKey[1024]="";
//	int				iChanNum;

	int iDreg = 0;
	char szMsgBuf[4096];
	char *pch;
	char szResponse[512];
	char szMsg[2048]="";
	int iType = -9;
	int iRet;
	bool bConnOK = true;


	errBuf[0]='\0';

	sprintf(errBuf, "[MSG]: Incomming Socket Fd :: %d", fd);
	McDbgLog(LOGLEVEL7, errBuf);

	
	iMsg	= new Message(fd);
	Message *oMsg = new Message(fd);

	/* HTTP negotiation with key
	*/
	int iNegotiated = 0;
	
	setRcvTimeOut(fd,250);

	memset(szMsgBuf, '\0',sizeof(szMsgBuf));
	sz = 0;
	pch = szMsgBuf;
	while(true)
	{
		try
		{
			if ( iMsg->recvPktRaw(pch, 1) < 0 )
			{
				{
					sprintf(errBuf, "[WARNING] Connection Breaks Fd :: [%d] RetCode :: [%d]", fd, sz);
					McDbgLog(LOGLEVEL2, errBuf);
					goto GET_OUT;
				}
			}

				pch++;
				sz++;
				if ( sz > 4 && szMsgBuf[sz-4] == '\r' && szMsgBuf[sz-3] == '\n' && szMsgBuf[sz-2] == '\r' &&  szMsgBuf[sz-1] == '\n')
					break;
				if ( sz > sizeof(szMsgBuf))
					break;
		}
		catch(...)
		{
			McDbgLog(LOGLEVEL0, "[ERROR] exception caught");
			goto GET_OUT;
		}
	}
	pch = strstr(szMsgBuf,"ey: ");
	if ( pch != NULL)
	{
		pch = pch+4;
		strcpy(szKey, pch);
		pch = strstr(szKey,"\r\n");
		if ( pch != NULL)
		{
			*pch = '\0';
		}
	}
	else
	{
		// No key available
		goto GET_OUT;
	}



	strcat(szKey, pszGUID);

	/* make sh1 hash */
    char szHashBytes[256];

	ComputeHash(szKey, szHashBytes);

	sprintf(szResponse,"HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",szHashBytes);
	oMsg->sendPktRaw(szResponse, strlen(szResponse));

//	GetDecodedData(szMsgBuf1, sz);

//	setRcvTimeOut(fd,120000);


	/*set keep alive for the client socket*/

	while (bConnOK)
	{
		try
		{

			while(true)
			{
				setRcvTimeOut(fd,120000);
				memset(szMsgBuf, '\0',sizeof(szMsgBuf));
				//	char szMsgBuf1[256];
				sz = 0;
				pch = szMsgBuf;
				if ( (iRet = iMsg->recvPktRaw(pch, 2)) < 0 )
				{
					if ( iRet == -4)
					{
						memset(szMsgBuf, '\0',sizeof(szMsgBuf));
						//	char szMsgBuf1[256];
						sz = 0;
						pch = szMsgBuf;

						continue;
					}
					else
					{
						bConnOK = false;
						break;
					}
				}
/*
**/
				int k = (byte) szMsgBuf[1] - 128;
				int j = 0; // byte read
				pch = pch+2;

				if (k <= 125)
				{
					j = 2;
				}
				else if (k == 126)
					{
						// read 2 more
						if ( (iRet = iMsg->recvPktRaw(pch, 2)) < 0 )
						{
							if ( iRet == -4)
							{
								continue;
							}
							else
							{
								bConnOK = false;
								break;
							}
						}
						pch =pch+2;
						j = 4;
					}
					else if ( k == 127)
						{
							//read 8 more
							if ( (iRet = iMsg->recvPktRaw(pch, 8)) < 0 )
							{
								if ( iRet == -4)
								{
									continue;
								}
								else
								{
									bConnOK = false;
									break;
								}
							}
							pch = pch + 8;
							j=10;
						}
					else
					{
						continue;	
					}

/*
*/
				sz = GetDataLen(szMsgBuf);

				if ( sz == 0)
					continue;

				if ( (iRet = iMsg->recvPktRaw(pch, sz-j)) < 0 )
				{
					if ( iRet == -4)
					{
						memset(szMsgBuf, '\0',sizeof(szMsgBuf));
						//	char szMsgBuf1[256];
						sz = 0;
						pch = szMsgBuf;

						continue;
					}
					else
					{
						bConnOK = false;
						break;
					}
				}
				szMsgBuf[sz] = '\0';
				GetDecodedData(szMsgBuf, sz);
				printf("Data size=%d, Data=%s\n", strlen(szMsgBuf), szMsgBuf);

		
				iType = -9;
				iType = getTypeNMsg(szMsgBuf, szMsg);
				switch(iType)
				{

					case 21:
					case MSG_CAPTURE_REGISTER_AGENT: //	320
						userIndex = processAddUser(fd, 1);
						break;
					case MSG_CAPTURE_DEREGISTER_AGENT: //	321
						processDelUser(fd, 1);
						break;

/*					case MSG_CAPTURE_START:	//		322
						processCaptureStart(fd, szMsg);
						break;
					case MSG_CAPTURE_STOP: //		323
						processCaptureStop(fd, szMsg, 1);
						break;
					case MSG_CAPTURE_MULTIPART_START: //		324
						processCaptureMultpartStart(fd, szMsg,1);
						break;
					case MSG_CAPTURE_IAMALIVE://		325
						processClientAlive(fd, szMsg, 1);
						break;
					case MSG_CAPTURE_IDLE: // Capturing app sends msg as recording is stopped
						processCaptureEndMsg(fd, szMsg);
						break;
					case MSG_CAPTURE_LIST:
						processCaptureList(fd, szMsg, 1);
						break;
					case MSG_CAPTURE_STATE:
						processCaptureState(fd, szMsg, 1);
						break;
					case MSG_HOLD_RECORDING:
						processHoldRecording(fd, szMsg, 1);
						break;
					case MSG_UNHOLD_RECORDING:
						processUnholdRecording(fd, szMsg, 1);
						break;
*/
					default:

						break;
				}
				/* process */
			}
			
		}
		catch(...)
		{
			McDbgLog(LOGLEVEL0, "[ERROR] exception caught");
			continue;
		}

		
	}/*while (true)*/

	{
		delUser(fd);
	}

	/* [26-12-2013]	[SG]	*/
/*	if ( iAgentIndex >= 0 )
		delAgent(iAgentIndex, iDreg);
*/
GET_OUT:

	iMsg->closeClient();
	delete iMsg;
	delete oMsg;
//	iClientCtr--;
	return RET_OK;

}/*DWORD WINAPI processRequest(void * tcpFd)*/

/*****************************************************************************
*MessageFormat: <type>,<Message>
*******************************************************************************/
int getTypeNMsg(char *pszMsgBuf, char *pszMsg)
{
	char *pch;
	int iType =-9;

	try
	{

		pch = strchr(pszMsgBuf, ',');

		if (pch == NULL)
			return atoi(pszMsgBuf);
		*pch = '\0';
		pch++;
		strcpy(pszMsg, pch);
		iType = atoi(pszMsgBuf);
	}
	catch(...)
	{
		return iType;
	}
	return iType;
}


/*************************************************************************************
*
**************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"sndWebPkt"
int sndWebPkt(int iFd, int iType, char *pszMsg)
{
	int iRc = 0;
	char szBuf[1024];
	byte *b;
	Message *oMsg = new Message(iFd);

	sprintf(szBuf,"%d;%s", iType, pszMsg);

	b = GetEncodedData(szBuf, strlen(szBuf));

	iRc = oMsg->sendPktRaw((char *)b, strlen((char *)b));

	delete b;
	delete oMsg;
	

	return iRc;


}
