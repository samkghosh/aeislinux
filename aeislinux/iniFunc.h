/***************************************************************************************************
* FileName: IniFunc.h
*
* Purpose:	This file contains the functions that are required to read, write and 
*			retrieve info from a ini file.
*
* Copyright (C) 2015, Alliance Infotech Pvt Ltd, New Delhi
*
* History
*	[00]	[21-01-2015]	Created
****************************************************************************************************/
#ifndef __SYNINI_FUNC_H__
#define __SYNINI_FUNC_H__

int nGetPrivateProfileString(char *pszSec, char *pszKey, char *pszDef, char *pszRet, int iSize, char *pszFileName);
int nGetPrivateProfileInt(char *pszSec, char *pszKey, int, char *pszFileName);

int ReadIni();

bool bIsNumeric(char *szInput);



bool bReadINIEntry(char *szSectName, char *szEntryName, char *szRet, char *szDef, int iSize, char *szName);

bool bReadINIEntryint(char *szSectName, char *szEntryName, int *piRet, int iDef, char *szName);

bool bFileExists(char *szFile);

bool bWriteINIEntry(char *szSectName, char *szEntryName, char *szFName, char *szValue);



void *periodicINIUpdate(void *p);
#endif	
