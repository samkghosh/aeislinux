
//#include "stdafx.h"
//#include <linux/io.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "logs3.h"
#include <signal.h>

//#include "egLm.h"
//#include "CAITService.h"
#include "ArcaneSyncService.h"

#include "ssglobal.h"

#include "iniFunc.h"

#define MAX_PATH	255


void *	ArcaneSyncService(void *);

//CEgLm		gLicense("AEISServer");

DataBase dbMain;
bool readIni();
 extern char *dos2UnixPath(char *pszPath);

int initReCeiver(int myPortNum);

char gszExePath[256];

void closeAll(int sig);

/* [08-03-2017]	[SG]	*/
int initUserTable();
//int initWebComms(int);
int initStatus();
int setLastFileUpdateState(char *pszSiteName);
int setRecCount(char *, int);

/* [19-04-2017]	[SG]	*/
void *checkStatusAndTakeAction(void *p);
long giLastStatusUpdate;
bool StopMyService(char *pszService);


//CString GetTechStaffMobileNo( int nZoneID, int nAreaID, int nCompType, int nCompNature, int nLevel)  ;


//#undef SERVICES
//#define SERVICES
/***************************************************
** function: 
**
** purpose : 
**
** Input:			`
**
** Return Values:    
**
** Description
**
****************************************************/
#undef __FNNAME__
#define __FNNAME__ "Main"
int	main(int argc, char *argv[])
{
	char			*pDependency[] = {NULL};//{"MySQL",NULL };
	char *pch;
	pthread_t thId;
	int arg=0;	

	signal(SIGABRT, closeAll);
	signal(SIGFPE,  closeAll);
	signal(SIGILL,  closeAll);
	signal(SIGINT,  closeAll);
	signal(SIGSEGV, closeAll);
	signal(SIGTERM, closeAll);
	signal(SIGHUP, closeAll);

	strcpy(gszExePath, argv[0]);
	
	pch = strrchr(gszExePath, '\\');

	if ( pch != NULL)
	{
	/* [25-04-2016]	[SG]	*/
		*pch = '\0';


	}
	else
	{
		strcpy(gszExePath,".");
	}

	sprintf(gszINIFileName, "/etc/arcanesync.ini");
	if ( access(gszINIFileName,0) == -1)
	{
		fprintf(stderr,"INI File Not found[%s].\nExiting...\n\n", gszINIFileName);
		return 0;
	}
	//_beginthread(ArcaneSyncService, 0, NULL);
         if( pthread_create(&thId,NULL,ArcaneSyncService,(void *)arg)==-1)
         {
               fprintf(stderr,"Error in creating thread: %d", errno);
         }

	printf("Started***\n");

	giProcessFlag =1;
	while( giProcessFlag ==1)
	{
		sleep(5);
	}
	return	0;
}



bool checkServicve();

/***************************************************
** function: 
**
** purpose : 
**
** Input:			
**
** Return Values:    
**
** Description
**
****************************************************/
#undef __FNNAME__
#define __FNNAME__ "ArcaneSyncService"
void *ArcaneSyncService(void * none)
{
	int i;
	
	char szSQL[512];
	//CStringArray arrFields[MAX_SITES];
	_CArray2D recArray;
	int nRecordsFetched;
	T_SiteInfo siteInfo;
	char szLog[MAX_PATH];
	pthread_t thId;	
	int arg=0;

	/* Reading INI Settings */
	readIni();

//	dbgLog.initLogs(gszLogDir, "SYNC", true, giLogLevel);

	giProcessFlag=1;

	McDbgLog(LOGLEVEL0, "*****************Starting ******************");

	sprintf(szLog, "Executable path=%s", gszExePath);
	McDbgLog(LOGLEVEL0, szLog);

#if 0
	/* [31-10-2012]	[SG]	License Added*/
	/* [27-09-2016]	[SG]	Set license path */
//		gLicense.setLicensePath(gszExePath);
		int rc = gLicense.check();
		switch(rc)
		{
			case EGLM_INVALID:
				sprintf(szLog,"[WARNING] License is not valid. Signature or Host id is not correct");
				break;
			case EGLM_NOENTRYFOUND:
				sprintf(szLog,"[WARNING] No License entry found for this product");	
				break;
			case EGLM_TIMEEXPIRED:
				sprintf(szLog,"[WARNING] Time has expired");
				break;
		}

		if ( rc != EGLM_OK)
		{
			McDbgLog(LOGLEVEL0, szLog);
//			MessageBox( NULL, szLog,"ArcaneLite(TM) License Warning",MB_OK);
			return;
		}

		/*no of channel to license*/
		int iLicenseChanCount = gLicense.getCount();


		if ( giNumOfSites == 0 )
		{
			giNumOfSites = iLicenseChanCount;
		}
		else
			if ( iLicenseChanCount < giNumOfSites )
				giNumOfSites = iLicenseChanCount;


		McDbgLog(LOGLEVEL1,"Licence Cheked");
#else
		giNumOfSites = 100;

#endif


	/* check if the database service is started or not */
	while(true)
	{
		if ( dbMain.Connect(gszDBIPAddress, gszUID, gszPWD, gszDBName) != DB_SUCCESS)
		{
			McDbgLog(LOGLEVEL1, "Database is Not Initialised");
			sleep(5);
		}
		else
			break;
	}
	McDbgLog(LOGLEVEL2, "Database is initialised");
	
	initReCeiver(giPortNum);

	// call the thread to sync loglevel
	//_beginthread(updateCfgValue, 0, NULL);

        if( pthread_create(&thId,NULL,periodicINIUpdate,(void *)arg)==-1)
	{
		fprintf(stderr,"Unable to create thread for updateCfgValue: %d\n", errno);
	}
	/* [08-03-2017]	[SG]	*/
	initUserTable();
	
	initStatus();

//	initWebComms(5679);


	/* [19-04-2017]	[SG]	Check if no communication is happening then take action */
	//_beginthread(checkStatusAndTakeAction,0,NULL);
        if( pthread_create(&thId,NULL,checkStatusAndTakeAction,(void *)arg)==-1)
	{
		fprintf(stderr,"Unable to create thread for checkStatusAndTakeAction: %d\n", errno);
	}


	while (giProcessFlag == 1)
	{
//fprintf(stderr,"Inside while..\n");
		try
		{
			McDbgLog(LOGLEVEL1,"Going To Process Upload Transacion");
			/* get the Site information from the database */
			sprintf(szSQL, "SELECT v_SiteName, v_DBServerIp, v_DBUserId, v_DBPwd, v_DBName, v_UploadPath from tbl_siteinfo");
			nRecordsFetched = 0;

//fprintf(stderr,"Going to Fetch\n");
			if ( dbMain.FetchRecord(szSQL,&recArray, giNumOfSites,&nRecordsFetched) != -1)
			{
//fprintf(stderr,"Number of Sites Fetched=%d\n", nRecordsFetched);

				for (i =0; i < nRecordsFetched; i++)
				{
					
					sprintf(siteInfo.szSiteName,"%s", recArray.get(i+1,1));
					sprintf(siteInfo.szDbServerIP,"%s", recArray.get(i+1,2));
					sprintf(siteInfo.szUserId,"%s", recArray.get(i+1,3));
					sprintf(siteInfo.szPwd,"%s", recArray.get(i+1,4));
					sprintf(siteInfo.szDBName,"%s", recArray.get(i+1,5));
					sprintf(siteInfo.szUploadPath,"%s", recArray.get(i+1,6));



					sprintf(szLog,"UploadPath=%s,SiteName=%s,DBIP=%s,UID=%s:%s,DBName=%s", siteInfo.szUploadPath,
																							siteInfo.szSiteName,
																							siteInfo.szDbServerIP,
																							siteInfo.szUserId,
																							siteInfo.szPwd,
																							siteInfo.szDBName);
					McDbgLog(LOGLEVEL2, szLog);
//fprintf(stderr,"%s\n", szLog);
try
{
					ProcessSyncInfo(&siteInfo); 
}
catch(...)
{
	fprintf(stderr,"Caught Excption..\n");
}
//fprintf(stderr,"After Process\n");

	/***********************************************************************/
				}

			}
			else
			{
#ifndef SERVICES
printf("Number of Sites Fetched=%0\n", nRecordsFetched);

#endif

			}
		
			sleep(120);
		}
		catch(...)
		{
			McDbgLog(LOGLEVEL0,"Exception Caught..");
			sleep(120);
		
		}
	}

	
	return NULL;																				/* return from function */
}





/********************************************************************************
*Function name -  ProcessSyncInfo
********************************************************************************/
#undef __FNNAME__
#define __FNNAME__ "ProcessSyncInfo"
int ProcessSyncInfo(T_SiteInfo *pSiteInfo)
{
//	bool del;
	//long n;
	char szFile[128];
	char szSQL[2048];
	char szUpldFile[256];
	char szLog[2048];
	int i;
	char szKey[64];
	int iCount;
	int iError;
	int nRecordsFetched;
	char szB[32];
	char *pch;	
	DataBase *db=NULL;
	_CArray2D recArray;

	struct dirent *de;
	DIR *dr;

	try
	{
		dos2UnixPath(pSiteInfo->szUploadPath);
		sprintf(szFile,"%s/transaction", pSiteInfo->szUploadPath);
		sprintf(szLog,"Find files in:%s", szFile);
		McDbgLog(LOGLEVEL2, szLog);

		if ( (dr = opendir(szFile)) == NULL)
		{
			iError= errno;
			if ( iError != 2)
			{
				sprintf(szLog,"Unable to Open Fo;der for file,[%s], _error=%d]", szFile, iError);
			
				McDbgLog(LOGLEVEL0, szLog);
			}
			sprintf(szLog,"Open dir for:%s Success", szFile);
			McDbgLog(LOGLEVEL2, szLog);

			if ( db == NULL )
			{
				sprintf(szLog,"DB Is Null");
				McDbgLog(LOGLEVEL2, szLog);
				db = new DataBase();

				if ( db->Connect(pSiteInfo->szDbServerIP, pSiteInfo->szUserId, pSiteInfo->szPwd,pSiteInfo->szDBName) == DB_SUCCESS)
				{
					sprintf(szLog,"DB Open Success");
					McDbgLog(LOGLEVEL2, szLog);
					sprintf(szSQL,"select count(d_starttime) from tbl_recordinglog where d_starttime >= CURDATE()");
					sprintf(szLog,"Get reccount:%s", szSQL);
					McDbgLog(LOGLEVEL2, szLog);
					nRecordsFetched = 0;
					if (db->FetchRecord(szSQL, &recArray, 1, &nRecordsFetched) == DB_SUCCESS)
					{
						if ( nRecordsFetched > 0)
						{
								sprintf(szB,"%s",recArray.get(1,1));

								sprintf(szLog,"Going to set rec count: %s for site:%s", szB, pSiteInfo->szSiteName);
								McDbgLog(LOGLEVEL2, szLog);
								setRecCount(pSiteInfo->szSiteName, atoi(szB));
							}
							else
							{
								McDbgLog(LOGLEVEL2, "Rec count is zero");
							}
						}
						else
						{
								McDbgLog(LOGLEVEL2, "Unable to Fetch record");
						}
						db->CloseDB();
					}
				}
				sprintf(szLog,"Unable to open Folder");
				McDbgLog(LOGLEVEL2, szLog);
				return -1;
			}
			else
			{
				McDbgLog(LOGLEVEL2,"Directory opened Successfully\n");
			}

			while ( (de = readdir(dr)) != NULL)	
			{
				if ( strstr(de->d_name, ".upld") == NULL)
					continue;	
				sprintf(szUpldFile,"%s/transaction/%s",pSiteInfo->szUploadPath, de->d_name);

				McDbgLog(LOGLEVEL3, szUpldFile);
#if 1
				if ( db == NULL )
				{
					db = new DataBase;

					if ( db->Connect(pSiteInfo->szDbServerIP, pSiteInfo->szUserId, pSiteInfo->szPwd,pSiteInfo->szDBName) != DB_SUCCESS)
					{
						McDbgLog(LOGLEVEL1, "Unable to Initialise Database");

						break;
					}
					else
					{
						sprintf(szLog,"DB Opened: %s@%s:%s", pSiteInfo->szUserId, pSiteInfo->szDbServerIP, pSiteInfo->szDBName);
						McDbgLog(LOGLEVEL2, "nitialise Database");

					}
				}
			


				if ( nGetPrivateProfileString("MAIN", "SQL", "", szSQL, sizeof(szSQL), szUpldFile) <=0)
				{
					if ( nGetPrivateProfileString("MAIN", "SQLMC", "", szSQL, sizeof(szSQL), szUpldFile) > 0)
					{
						if (db->ExecuteQuery(szSQL) != DB_SUCCESS)
						{
							sprintf(szLog,"UEQ: %s", szSQL);
							McDbgLog(LOGLEVEL0, szLog);

						}
					}
//fprintf(stderr,"After SQL\n");

					iCount = nGetPrivateProfileInt("LOCAL", "Count", 0, szUpldFile);
				
					for(i = 0; i < iCount; i++)
					{
						sprintf(szKey,"SQL%d", i);
						if ( nGetPrivateProfileString("LOCAL", szKey, "", szSQL, sizeof(szSQL), szUpldFile) > 0)
						{
							if ( db->ExecuteQuery(szSQL) != DB_SUCCESS)
							{
								sprintf(szLog,"UEQ: %s", szSQL);
								McDbgLog(LOGLEVEL0, szLog);

							}
						}	
					}	
				}
				else
				{
					if ( db->ExecuteQuery(szSQL) != DB_SUCCESS)
					{
								sprintf(szLog,"UEQ: %s", szSQL);
								McDbgLog(LOGLEVEL1, szLog);
					}
					else
					{
						if ( nGetPrivateProfileString("MAIN", "AUTO", "", szSQL, sizeof(szSQL), szUpldFile) > 0)
						{
								if ( db->ExecuteQuery(szSQL) != DB_SUCCESS)
								{
									sprintf(szLog,"UEQ: %s", szSQL);
									McDbgLog(LOGLEVEL0, szLog);
			
								}
			
						}
					
						if ( nGetPrivateProfileString("MAIN", "SIZE", "", szSQL, sizeof(szSQL), szUpldFile) > 0)
						{
							if ( db->ExecuteQuery(szSQL) != DB_SUCCESS)
							{
									sprintf(szLog,"UEQ: %s", szSQL);
									McDbgLog(LOGLEVEL0, szLog);
							}
						}

						if ( nGetPrivateProfileString("MAIN", "RECCOUNT", "", szSQL, sizeof(szSQL), szUpldFile) > 0)
						{
							if ( db->ExecuteQuery(szSQL) != DB_SUCCESS)
							{
									sprintf(szLog,"UEQ: %s", szSQL);
									McDbgLog(LOGLEVEL0, szLog);

							}
						}

						/* [14-12-2012]	[SG]	Update last sync Time */

						if ( pSiteInfo->szSiteName[0] != '\0')
						{
							
							sprintf(szSQL, "UPDATE tbl_siteinfo SET d_LastSyncTime = NOW(), i_SiteStatus = '1', d_StatusDate = NOW() WHERE v_SiteName = '%s'", pSiteInfo->szSiteName);
							if (dbMain.ExecuteQuery(szSQL) != DB_SUCCESS)
							{
									sprintf(szLog,"UEQ: %s", szSQL);
									McDbgLog(LOGLEVEL0, szLog);

							}
						}

					}
				}

			sprintf(szLog,"Successfully Uploaded: File=%s", szUpldFile);
			McDbgLog(LOGLEVEL3, szLog);
		
			/* [09-03-2017]	[SG]	*/
			setLastFileUpdateState(pSiteInfo->szSiteName);

			// remove the file
			unlink(szUpldFile);
#endif
		}
		closedir(dr);
McDbgLog(LOGLEVEL2, "Direcoty read finished..");
#if 0
		/* [10-03-2017]	[SG]	*/
		if ( db != NULL)
		{
			sprintf(szSQL,"select count(d_starttime) from tbl_recordinglog where d_starttime >= CURDATE()");
			nRecordsFetched = 0;
			recArray.ReleaseAll();
			if (db->FetchRecord(szSQL, &recArray, 1, &nRecordsFetched) == DB_SUCCESS)
			{
				if ( nRecordsFetched > 0)
				{
					sprintf(szB,"%s",recArray.get(1,1));

					setRecCount(pSiteInfo->szSiteName, atoi(szB));
					//fprintf(stderr,"Before Release...\n");
					recArray.ReleaseAll();
					//fprintf(stderr,"After Release...\n");
				}
			}
		}
		else
		{
			setRecCount(pSiteInfo->szSiteName, 0);
		}
#endif

	}
	catch(...)
	{
		McDbgLog(LOGLEVEL0,"Exception Caucht....");

	}


	if ( db !=NULL)
	{
//fprintf(stderr,"1...\n");
		db->CloseDB();
//fprintf(stderr,"2...\n");
		delete db;
//fprintf(stderr,"3...\n");
	}

	McDbgLog(LOGLEVEL2,"Exit");

//fprintf(stderr,"Returning...\n");
	return 0;
}




/*********************************************************
*
**********************************************************/
void Wait(int t)
{
	int iCount =0;

	while(giProcessFlag)
	{
		sleep(2);
		if ( iCount * 2 > t)
			break;
		iCount++;
	}
}


/************************************************************************************************
*
*************************************************************************************************/
#undef __FNNAME__
#define __FNNAME__ "checkStatusAndTakeAction"
void *checkStatusAndTakeAction(void *p)
{
	giLastStatusUpdate = -1;
	int iStatusInterval = -1;

#if 0
//TBD	
	McDbgLog(LOGLEVEL2,"*************** Started *****************");
	/* Read Status check Interval in min*/
	iStatusInterval = nGetPrivateProfileInt("Main","CheckStatus",0,gszINIFileName);
	
	
	if ( iStatusInterval > 0)
	{
		iStatusInterval = iStatusInterval * 60;

		while(giProcessFlag)
		{
			try
			{

				if ( giLastStatusUpdate > 0)
				{
						if ( (time(NULL) - giLastStatusUpdate) > iStatusInterval)
						{
							/* No status is coming from site. clean up */
							McDbgLog(LOGLEVEL0,"********** No Status rcvd for long. Let me cleanup.");
							StopMyService("AEISServer");
							exit(0);

						}
				}
				else
				{
					Wait(10);				
				}
			}
			catch(...)
			{
				McDbgLog(LOGLEVEL0, "Exception...");
			}

			Wait(10);

		}
	}
#endif
	McDbgLog(LOGLEVEL2,"*************** Stopped*****************");

}
/********************************************************************
*********************************************************************/
#undef __FNNAME__
#define __FNNAME__	"closeAll"
void closeAll(int sig)
{
	char szLog[256];

	sprintf(szLog,"Got Signal=%d\n", sig);
fprintf(stderr,"%s\n",szLog);

	McDbgLog(LOGLEVEL0,szLog);

	if (sig == SIGABRT || sig == SIGILL || sig == SIGTERM )
	{
		McDbgLog(LOGLEVEL0,"**************** Exiting Application***********");
		giProcessFlag =0;
		sleep(5);
	}
}
