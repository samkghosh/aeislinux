/***************************************************************************************************
* FileName: dbfunction.h
*
* Purpose:	Maintain database related work
*
* Copyright (C) 2008, Alliance Infotech Pvt Ltd, New Delhi
*
* History
*	[00]	[23-04-2008]	Created
*	[01]    [03-12-2012]    Created``  
****************************************************************************************************/
#ifndef __SYNDBFUN_H__
#define __SYNDBFUN_H__

#define		DB_SUCCESS				 0  //function successfully returned
#define		DB_FAIL					-1  //general function failure
#define		DB_CONNECTFAIL			-2  //connection with the database failed
#define		DB_NORECORDS			-3	//No record found in the DB table
#define		ODBC_CONNECT_FAIL		-4
#define		ODBC_QUERY_FAIL			-5

int	InitDB(char *pszIPAddress, char *pszUserName, char *pszPassword, char *pszDBName);

int ExecSQL(char* strSQL,char *pszIPAddress, char *pszUserName, char *pszPassword, char *pszDBName);

int	InitDBMain();

int ExecSQLMain(char* strSQL);//,char *pszIPAddress, char *pszUserName, char *pszPassword, char *pszDBName);

int ExecSelectQueryMain(char *	szSQL,CStringArray *arrFields,int nMaxRecordsFetch,int *nRecordsFetched,bool	bDoLock/*=true*/);

int CloseDB(void);

BOOL SelectInfo();

int CloseDBMain(void);

/* [10-03-2017]	[SG]	*/
int ExecSelectQuery(char *	szSQL,CStringArray *arrFields,int nMaxRecordsFetch,int *nRecordsFetched,bool	bDoLock, 
					char *pszDbServerIP, char *pszUserId, char *pszPwd,char *pszDBName);


#endif