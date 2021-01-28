/*******************************************************************
* fileName: ssextern.h
*********************************************************************/
#ifndef __SSEXTERN_H__
#define __SSEXTERN_H__


extern int giProcessFlag;

extern Logs dbgLog;
extern char gszLogDir[256];
extern char giLogLevel;
extern int giNumOfSites;
extern int giCurrSiteNum;
extern int giPortNum;
//extern T_SiteInfo siteInfo[MAX_SITES];

extern char gszINIFileName[256];
extern char gszBasePath[256];

/* [27-09-2016]	[SG]	*/
extern char gszFTPPath[256];


#ifndef INI_NAME
#define INI_NAME	"/etc/SyncServ.ini"
#endif
/* [27-09-2016]	[SG]	*/
extern char gszFTPPath[256];

extern char gszDBIPAddress[256];
extern char gszUID[64];
extern char gszPWD[64];
extern char gszDBName[64];



#endif
