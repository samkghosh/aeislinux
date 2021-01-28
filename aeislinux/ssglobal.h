/*******************************************************************
* fileName: ssglobal.h
*********************************************************************/
#ifndef __SSGLOBAL_H__
#define __SSGLOBAL_H__


#ifndef INI_NAME
#define INI_NAME	"SyncServ.ini"
#endif



//HANDLE EvtHandle;					/* Extern Event handle */ 


int giProcessFlag=0;

Logs dbgLog;
char gszLogDir[256];
char giLogLevel;
int giNumOfSites=0;
int giPortNum;
//T_SiteInfo siteInfo[MAX_SITES];

char gszINIFileName[256];
char gszBasePath[256];

/* [27-09-2016]	[SG]	*/
char gszFTPPath[256];

char gszDBIPAddress[256];
char gszUID[64];
char gszPWD[64];
char gszDBName[64];
#endif
