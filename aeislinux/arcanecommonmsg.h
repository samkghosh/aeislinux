/*
* FileName: ArcaneCommonMsg.h
*/

#ifndef __ARCANECOMMONMSG_H__
#define __ARCANECOMMONMSG_H__



/*************************MESSAGES SENT BY CLIENT***************************/
/*


/*******************************************
* MAX Values
********************************************/
#define MAX_CHANINFOSIZE		240
/*__________________________________________________________________
Messages for sending User Commands to server
__________________________________________________________________
*/
#define BYTE	unsigned char

#define		MSG_USER_LOGIN					1
#define		MSG_USER_LOGOUT					2
#define		MSG_USER_SWITCH_CHAN			3
#define		MSG_USER_DIALNCONNECT			4
#define		MSG_USER_ADD_CHANINFO			5
#define		MSG_USER_LINE_HOOKINFO			6
#define		MSG_USER_UPDATE_TAPPINGNO		7



#define		MSG_USER_START_CHANNEL			8
#define		MSG_USER_STOP_CHANNEL			9

#define     MSG_USER_START_RECORD			10 // Start record manually
#define		MSG_USER_STOP_RECORD			11 //Stop record manually

#define     MSG_USER_CHANSET_REFRESH        12


#define		MSG_USER_STOP_COMM				13
#define     MSG_USER_START_COMM				14

//[RS] [Date 12-06-2009]// MSG for ComPort
#define     MSG_COMPORT_ACTIVITY			15
#define		MSG_USER_CHANNEL				16
#define		MSG_USER_CHANNEL_STATE			17


/* [07-05-2012]	[SG]	*/
#define		MSG_FILE_IO_EXISTS				20
#define		MSG_FILE_IO_UNLINK				21
#define		MSG_FILE_IO_COPY				22
#define		MSG_FILE_IO_GET					23
#define		MSG_FILE_IO_PUT					24
#define		MSG_FILE_IO_MM_STREAM			25
#define		MSG_FILE_IO_MM_GETTYPE			26

#define		MSG_FILE_IO_NOTEXISTS			27
#define		MSG_FILE_IO_COPY_END			28



/* [06-01-2013]	[SG]	*/
#define MSG_AGENT_IAM_ALIVE					29

/* [13-07-2010]	[sg]	*/

/* Message Type For Logger */
#define MSG_TAPI_INCOMMING_CALL		101
#define MSG_TAPI_OUTGOING_CALL		102
#define MSG_TAPI_CALL_CONNECTED		103
#define MSG_TAPI_CALL_DISCONNECTED	104
#define MSG_TAPI_CALL_BARGEIN		105

/* [19-01-2011] */
/* Message tt login Agent */
#define MSG_AGENT_LOGIN				201
#define MSG_AGENT_LOGOUT			202
#define MSG_AGENT_LOGIN_OK			203
#define MSG_AGENT_LOGIN_NOTOK		204


/* [20-09-2013]	[SG]	*/
#define MSG_GET_CHAN_SETTING		301
#define MSG_SET_CHAN_SETTING		302
#define MSG_MORE					303
#define MSG_LESS					304
#define MSG_GET_CHAN_SETTING_AUDIO	305
#define MSG_SET_CHAN_SETTING_AUDIO	306


#define MSG_START					307
#define MSG_EOD						308
#define MSG_NEXT					309
#define MSG_BREAK					310

/* [15-11-2014]	[SG]	*/
#define MSG_GET_INI					311
#define MSG_NEXT_INI				312
#define MSG_SET_INI					313
#define MSG_PROCESSING_ERROR		314

/*[02-04-2016]	[SG]	*/
#define MSG_CAPTURE_REGISTER_AGENT	320
#define MSG_CAPTURE_DEREGISTER_AGENT	321
#define MSG_CAPTURE_START		322
#define MSG_CAPTURE_STOP		323
#define MSG_CAPTURE_MULTIPART_START	324
#define MSG_CAPTURE_IAMALIVE		325
#define MSG_CAPTURE_GET_STATUS		326
#define MSG_CAPTURE_IDLE			327
#define MSG_CAPTURE_RECORDING		328
#define MSG_CAPTURE_LIST			329
#define MSG_CAPTURE_STATE			330

#define MSG_HOLD_RECORDING			331
#define MSG_UNHOLD_RECORDING		332
#define MSG_CAPTURE_RECORDING_DONE	333

#define MSG_USER_PAUSE_REC  18
#define MSG_USER_UNPAUSE_REC  19

/* [26-12-2013]	[SG]	*/
#define MSG_POPUP					401

/* [11-02-2020]	[SG]	*/
#define MSG_DIAL_N_PLAY				501
#define MSG_DIAL_N_PLAY_OK			502
#define MSG_DIAL_N_PLAY_NOK			503

/* User Type */
#define OPERATOR					1
#define SUPERVISOR					2
#define ADMIN						3
enum
{
	CS_IDLE =0,
	CS_INPROGRESS,
	CS_CONNECTED,
	CS_DISCONNECTED,
	CS_OFFERING,
	CS_HOLD,
	CS_QUED,
	CS_LASTITEM

};


/* [24-02-2014]	[SG]	*/

/* [11-04-2012]	[SG]	Added extn for encoded files */
#define EXTN_ENCODE		"aal"

//Structure for Login Message MSG_USER_LOGIN
typedef struct
{
	char	szUserId[32];
	int		nChanNum;
	char	szTappingNum[32];
	int		nStationId;
	char	szUserIp[32];

}T_LoginMsg;


//Structure for Login Message MSG_USER_SWITCH_CHAN
typedef struct
{
	int		nChanNum;

}T_SwitchChanMsg;

//Structure for Login Message MSG_USER_DIALNCONNECT
typedef struct
{
	int		nChanNum;
	char	szPhone[32];

}T_DialNConnectMsg;


//Structure for Login Message MSG_USER_ADD_CHANINFO
//Structer  for update tapping number MSG_USER_UPDATE_TAPPINGNO (send T_AddChanInfo stru);
typedef struct
{
	int		nChanNum;
	char	szTappingNum[16];

}T_AddChanInfo;


typedef struct
{

	int				nItem;
	T_AddChanInfo	pAddChanInfo[MAX_CHANINFOSIZE];

}T_AddChanInfoMsg;


//Structure for Login Message MSG_USER_LINE_HOOKINFO
typedef struct
{
	int	nValue;

}T_LineHookInfoMsg;

// [NRD] 140508
//Structure for start and stop recording manually

typedef struct
{
	int iChanNum;
}T_StartStopRecord;

//Structure for Updating channel data whenever any update occurs in Database
typedef struct
{
	int iChanNum;
	bool bAllChanUpdate;
}T_UpdateChanData;



/* [13-07-2010]	[SG]	Tapi Implementation */
/* Structure for the Call Info */
typedef struct
{
	char szANI[128];
	char szDNIS[128];
	int	 iCallDirection;	//1: IN, 2: OUT
}T_TAPI_CallInfo;

/* [13-07-2010]	[SG]	Tapi Implementation */
/* Structure for the Call Info */
typedef struct 
{
	char			szDN[32];
	char			szANI[128];
	char			szDNIS[128];
	int				iCallDirection;	// 1: incoming, 2: out going
	char			szDigitCollected[128];
	int				nCallState;
	//char			szCallerNo[20];
}T_TAPI_MsgInfo;


/*	[26-12-2013]	[SG]	*/

typedef struct 
{
	int				nCallDirection;	// 1: incoming, 2: out going
	int				nCallState;
	char			szDN[32];
	char			szIP[32];
	char			szANI[128];
	char			szDNIS[128];	
	char			szDigits[128];
	char			szSessionId[20];
	

}T_Call_MsgInfo;


/*************************MESSAGES SENT BY SERVER***************************/
/*
_______________________________________________________________
Messages for displaying information on for a particular channel
_______________________________________________________________
*/

//Msg to update voltage of a channel
//T_ChanMsg.szValue contains VOLTAGE show in the voltage box
#define		MSG_CHAN_VOLT					1
//Msg when server calls a number
//T_ChanMsg.szValue contains called number or nothing.
#define		MSG_CHAN_CALLED					2
//Msg when server recieves 
//T_ChanMsg.szValue contains called number or nothing.
#define		MSG_CHAN_RECIEVED				3
//T_ChaMsg.szValue contains nothing, show "Recording Started"
#define		MSG_CHAN_REC_STARTED			4	
//T_ChaMsg.szValue contains nothing, show "Hangup"
#define		MSG_CHAN_HANGUP					5
//T_ChaMsg.szValue contains nothing, show "Incomming Call"
#define		MSG_CHAN_INCOMING_CALL			6
//T_ChaMsg.szValue contains value, show as is it
#define		MSG_CHAN_CUSTOM_MSG				7


#define     MSG_SMS_ALERT					8

/*
_________________________________________________________________
Messages for sending replys to User Commands by server
__________________________________________________________________
*/
//No struct
#define		MSG_SERVER_ALREADY_LOGGEDIN				8
//No struct
#define		MSG_SERVER_EXCEEDS						9
//No struct
#define		MSG_SERVER_LOGIN_SUCCESS				10
//No struct
#define		MSG_SERVER_LOGOUT_SUCCESS				11
//No struct
#define		MSG_SERVER_SWITCH_CHAN_SUCCESS			12
//No struct
#define		MSG_SERVER_SWITCH_CHAN_NOTCHANGE		13

#define		MSG_SERVER_AFTER_SWITCH_CHAN			14	
	
#define		MSG_SERVER_UPDATE_TAPPINGNO_SUCCESS		15

#define		MSG_CHAN_VOLT_POWER						16	//msg to send all chan volt and power

#define		MSG_START_CHANNEL_SUCCESS				17
#define		MSG_STOP_CHANNEL_SUCCESS				18

/* 14-04-2011 */
#define		MSG_REC_DURATION						19


/* 02-03-2013]	[SG]	*/
#define		MSG_UPDATE_CHANS						20

/* [22-05-2015]	[SG]	*/
#define		MSG_UPDATE_EXTENSION_AGENT				21

/*********************************** Macros for Video Capturing *******************************/


#define		MSG_VIDEO_START_CAPTURING				1
#define		MSG_VIDEO_STOP_CAPTURING				2
#define		MSG_VIDEO_INIT_CAPTURING				3
#define		MSG_GET_FILE							4
#define		MSG_RCV_FILE							5
#define		MSG_RCV_END_FILE						6
#define		MSG_RCV_NO_FILE							7
#define		MSG_RCV_FILE_START						8
#define		MSG_GET_NEXT							9
/* [17-01-2014]	[SG]	*/
#define		MSG_REMOVE								10
/* [18-03-2016]	[SG]	added for multipart capturing */
#define		MSG_VIDEO_START_CAPTURING_MULTIPART		11

#define		MSG_VIDEO_START_CAPTURING_MULTIPART_NOCHK		12

/* [01-11-2014]	[SG]	[SG]	*/
/*#define		MSG_FTP_REQ								101
#define		MSG_FTP_START							102
#define		MSG_FILE_CRC							103
#define		MSG_FILE_CRC_ERROR						104
#define		MSG_FILE_SENT_OK						105
#define		MSG_FILE_SENT_NOK						106
*/
/* [01-11-2014]	[SG]	[SG]	*/
#define		MSG_FTP_REQ								101
#define		MSG_FTP_START							102
#define		MSG_FILE_CRC							103
#define		MSG_FILE_CRC_ERROR						104
#define		MSG_FILE_SENT_OK						105
#define		MSG_FILE_SENT_NOK						106
#define		MSG_FILE_TX_REMOTE_ERROR				107

#define		CAP_TYPE_NONE							0
#define		CAP_TYPE_VIDEO							1
#define		CAP_TYPE_STILL							2


#define		CAP_PORT_NO		55555
#define     MAXCHNKSZ       (4096-4)

typedef struct
{
	int size;
	char data[MAXCHNKSZ];
}T_FileBuffer;


//These 6 Messages Use a struct as define
typedef struct
{
	int		nChanNum;
	char	szValue[512];
}T_ChanMsg;


//Structure for after switching chanel
typedef struct
{
	char	szType[8]; //szType = "RCVD" or "CALLED"
	char	szPhNum[32];

}T_AfterSwitchChanMsg;


/***
[GI] 01052006
**/

typedef struct
{
	char	nChanNum ;			//int nChannel No
	char	nLineVoltage ;		//line voltage
	char	nLinePower ;		//line power
} T_ChanVoltPower ;


typedef	struct
{
	int					nTotalNoOfChan ;
	T_ChanVoltPower		pChanVP[120];
} T_ChanVoltPowerMsg ;

/**/

typedef struct
{
	int iUsed;
	char szTappingNum[32];
	int	iRecFormat;
	int iRecMode;
	/* Mobile Info */
	int iComPort;
	char szSerDevName[32];
	int	 iSpeed;
	char szMobileHangupString[32];
	bool	bCheckSMS;
}T_ChanInfo;




typedef struct
{
	int iUsed;
	/* various channel params */

	int					iRecMode;
	int					iRecFormat;
	int					iStream;

	bool				bUseScheduling;
	unsigned int		iFromSchedulingTime;
	unsigned int		iToSchedulingTime;


	/* AGC */
	bool				bAGC;
	int					iAGC_TMA;
	int					iAGC_AVCR;
	int					iAGC_DVCR;
	int					iAGC_MA;
	
	/* Activiry Detecting Level */
	bool				bACTD;
	int					iACTD_ThLow;
	int					iACTD_ThHigh;
	int					iACTD_MinSil;
	int					iACTD_MaxSil;
	int					iACTD_MinActdMs;
	int					iACTD_MaxActdMs;

	/* Voice Detection */
	int					bVD;
	int					iVD_ThPower;
	int					iVD_RelTh;
	int					iVD_ThFilterCnt;
	int					iVD_NoVoiceMin;
	int					iVD_Reset;
	int					iVD_VoiceMin;
	int					iVD_McMin;
	/* Voltage */
	bool				bLV;
	int					iLV_Low;
	int					iLV_High;
	int					iLV_DeglitchTimeMs;

	char				szTappingNum[32];
	char				szChannelName[32];
	/* Mobile Info */
	int					iComPort;
	char				szSerDevName[32];
	int					iSpeed;
	char				szMobileHangupString[32];
	char				szInitString[64];
	char				szAnswerString[16];

	bool	bCheckSMS;


}T_ChanProp;

	
/* Screen Capture Information */
	
typedef struct
{
	char szCaptureServerIP[32];
	char szComressionType[64];
	int  iCaptureType;
	int  iAudioMixing; /* 0: no, else yes */
	int  iMaxRecTime;  /* in sec */
	int  iFramePerSec;
	char szFileName[128];	
}T_ScreenCapProp;


//[RS][Date 18-02-2009]
enum
	{
		REC_MODE_VOX, 
		REC_MODE_HOOK,
		REC_MODE_MANUAL,
		REC_MODE_DTMF,
		REC_MODE_MOBILE,
		REC_MODE_PSEUDO,
		REC_MODE_IVR,
		REC_MODE_TRUNK,
		REC_MODE_RING,
		REC_MODE_FAX,
		REC_MODE_VOIP
		
	};


/* 19-01-2011 */
/* Message structure for Agent Login */
typedef struct
{
	char szPhoneIP[32];
	char szAgentId[32];
	char szAgentPwd[32];
	/* [30-12-2013]	[SG]	*/
	char szExtn[64];

}T_AgentInfo;


/* 14-04-2011 */
/* Message to send duration of recording */

typedef struct 
{
		short iChanNum;
		short iDuration;
}T_DURATION;

#define DURATION_REC_SZ		(sizeof(T_DURATION))

typedef struct
{
	short iNumOfRec;
	T_DURATION Duration[MAX_CHANINFOSIZE];
}T_MsgDuration;


/* [20-07-2012]	[SG]	*/
#define MSG_REMOTE_STATUS	1
#define MSG_SITE_STATUS		2
#define MSG_REC_REPORT		3

/* [17-06-2014]	[SG]	*/
#define MSG_REMOTE_STATUS_3	4


typedef struct
{
	BYTE iChanNum;
	BYTE iVolt;
	BYTE iStatus;
	char szExtn[16];
}T_RemoteChanStatus;

#define CHAN_STAT_SZ	(sizeof(T_RemoteChanStatus))
#define MAX_REM_CHAN_STAT	(4096/CHAN_STAT_SZ)

typedef struct
{
	char szSiteName[32];
	int iChanCount;
	int iStatus;
	T_RemoteChanStatus chanStat[MAX_REM_CHAN_STAT];
}T_RemoteStatus;


/* [20-11-2012]	[SG]	*/
typedef struct
{
	char szSiteName[32];
	unsigned long lFreeSpace; // in MB
	int	iDeviceStatus;	// 0: not active, 1: active/
	int iRecCount;
}T_SiteStatus;

/* [20-12-2012]	[SG]	*/
#define MAX_REPORT	30
typedef struct
{
	char szSiteName[32];
	int iCount;
	struct
	{
		char szDate[12];
		int iRecCount;
	}report[MAX_REPORT];
}T_RecCountReport;

//These 6 Messages Use a struct as define
typedef struct
{
	int iRecServerId;
	int		nChanNum;
	char	szValue[128];
}T_ChanMsgEx;


/* [28-02-2013] [VS] struc according to MS SQL server for processing message */

#define CHAN_STAT_V2_SZ	(sizeof(T_RemoteChanStatusV2))
#define MAX_REM_CHAN_STAT_V2	(4096/CHAN_STAT_V2_SZ)


typedef struct
{
	BYTE iChanNum; // Port number or channel number
	BYTE iVolt;		// voltage
	BYTE iStatus;	// status 0,1 or 2
	char szExtn[32];	// Extention number as defined
}T_RemoteChanStatusV2;



typedef struct
{
	union
	{
		char szGUID[38];  //main unique ID - reference to company id and site id
		struct
		{
			int  iCompanyID;
			int  iSiteInfoID;			
		}CSID; // structure to keep both company and site id;		
		
	}UID;// union to allocate memory for GUID or CSID at a time
	int						iChanCount;
	int						iStatus;
	unsigned long			lFreeSpace; //in MB

	T_RemoteChanStatusV2	ChanStat[MAX_REM_CHAN_STAT_V2];
}T_RemoteStatusV2;  //For checking Remote status 

/* [17-06-2014]	[SG]	*/
typedef struct
{
	union
	{
		char unused[4];  //main unique ID - reference to company id and site id
		struct
		{
			int  iCompanyID;
			int  iSiteInfoID;			
		}CSID; // structure to keep both company and site id;		
		
	}UID;// union to allocate memory for GUID or CSID at a time
	int						iChanCount; /* data for the channels are travelling, when current data < max chan */
	int						iStatus;
	unsigned long			lFreeSpace; //in MB
	int						iMaxChanCount;	/* total channel count in the system */

	T_RemoteChanStatusV2	ChanStat[MAX_REM_CHAN_STAT_V2];
}T_RemoteStatusV3;  //For checking Remote status 




typedef struct
{
	union
	{
		char szGUID[38];  //main unique ID - reference to company id and site id
		struct
		{
			int iCompanyID;
			int iSiteInfoID;			
		}CSID; // structure to keep both company and site id;		

	}UID;  // union to allocate memory for GUID or CSID at a time
	unsigned long	lFreeSpace; //in MB
	int				iDeviceStatus; // 0: not active, 1: active/
	int				iRecCount;
}T_SiteStatusV2; // To store Site status for checking it's condition 


typedef struct
{
	union
	{
		char szGUID[38];  //main unique ID - reference to company id and site id
		struct
		{
			int  iCompanyID;
			int  iSiteInfoID;
		}CSID; // structure to keep both company and site id;	

	}UID;  // union to allocate memory for GUID or CSID at a time
	int iCount;
	struct
	{
		char szDate[12];
		int  iRecCount;
	}Report[MAX_REPORT];
}T_RecCountReportV2;  // To store Recorded file info


/* [02-03-2013]		[SG]	*/
typedef	struct
{
	BYTE iChanId;
	BYTE iStatus;
	char szClip[18];
}T_CHANS;

typedef struct
{
	BYTE iRecServerId;
	BYTE	iCount;
	T_CHANS st[MAX_CHANINFOSIZE];
}T_CHAN_STATUS;
#define SZ_CHAN_HDR (sizeof(BYTE) * 2)
/*****************************************************************/

#if 1
/* [19-09-2013]	[SG]	*/
typedef struct
{
	BYTE i_recserverid; //` int(4) NOT NULL default '0',
	short i_channelid; //` int(4) NOT NULL default '0',
	char v_channelname[17]; //` varchar(16) default NULL,
	BYTE i_channelmode; //` int(4) NOT NULL default '0',
  	char v_tappingno[17]; //` varchar(16) default NULL,
	BYTE i_UseScheduling; //` int(1) default '0',
	char d_FromDateTime[20]; //` datetime default NULL,
	char d_ToDateTime[20]; //` datetime default NULL,
	char v_ACTD_ThresholdLow[4]; //` varchar(8) NOT NULL default '-51',
	char v_ACTD_ThresholdHigh[4]; //` varchar(8) NOT NULL default '-48',
	short i_ACTD_MinSilencems; //` int(8) NOT NULL default '40',
	int i_ACTD_MaxSilencems; //` int(8) NOT NULL default '30000',
	int i_ACTD_MinActivityms; //` int(8) NOT NULL default '40',
	int i_ACTD_MaxActivityms; //` int(8) NOT NULL default '10000',
	BYTE i_ACTD_IsEnable; //` int(1) NOT NULL default '0',
	short i_AGC_TMA; //` varchar(8) NOT NULL default '0',
	short i_AGC_AVCR; //` int(8) NOT NULL default '400',
	short i_AGC_DVCR; //` int(4) NOT NULL default '8',
	short i_AGC_MA; //` int(4) NOT NULL default '42',
	BYTE i_AGCIsEnable; //` int(1) default '0',
	short i_LVThresholdLow; //` int(4) default '-16',
	short i_LVThresholdHigh; //` int(4) default '16',
	short i_LVmsDeglitchTime; //` int(4) default '50',
	BYTE i_LVIsEnable; //` int(1) default '1',
	int i_Silencems; //` int(8) default '60',
	BYTE i_RecordingFormat; //` int(1) default '3',
	BYTE i_IsStreaming; //` int(1) default '1',
	BYTE i_PhoneConfiguration; //` int(1) default '1',
	short i_VDThresholdPower; //` varchar(8) default '-36',
	short i_VDRelativeThreshold; //` int(4) default '65',
	BYTE i_VDThresholdFilterCnt; //` int(2) default '5',
	short i_VDNoVoiceMin; //` int(8) default '1000',
	short i_VDReset; //` int(8) default '3000',
	short i_VDVoiceMin; //` int(8) default '100',
	short i_VDMachineMin; //` int(8) default '3000',
	BYTE i_VDIsEnabled; //` int(1) default '1',
	BYTE i_ComPortNo; //` int(4) default '0',
	short i_SecondaryChan; //` int(4) default '-1',
	short i_PlayGain; //` int(4) default '10',
	int		 i_VOXLevel; //` int(4) default '500',
	short i_VOXPresenseTime; //` int(4) default '5000',
	BYTE i_Used; //` int(1) default '1',
	short i_BargeInSense; //` int(4) default '-30',
	short i_RecordGain; //` int(4) default '-30',
	char v_AgentIP[16]; //` varchar(32) default '127.0.0.1',
	BYTE i_CaptureType; //` int(4) default '0',
	BYTE i_CompressorIndex; //` int(4) default '0',
	BYTE i_AudioVideoMix; //` int(4) default '0',
	short i_maxVideoTime; //` int(8) default '0',
	BYTE i_FramePerSec; //` int(4) default '1',
	char v_SerDev[16]; //` varchar(32) default NULL,
	short i_PortSpeed; //` int(4) default '9600',
	char v_HangUpString[16]; //` varchar(16) default 'No Carrier',
	BYTE i_CheckSms; //` int(1) default '0',
	char v_InitString[32]; //` varchar(32) default NULL,
	char v_AnswerString[16]; //` varchar(32) default NULL,
}T_Channels;

#endif
typedef struct
{
	short	iSiteId;
	short	iCompanyId;
	BYTE	iRecServerId;

	int iUsed;
	/* various channel params */

	short	iRecMode;
	BYTE	iRecFormat;

	char	szTappingNum[32];
	char	szChannelName[32];

	BYTE	iStream;

	BYTE	bUseScheduling;
	unsigned int		iFromSchedulingTime;
	unsigned int		iToSchedulingTime;


	/* AGC Audiocodes */
	BYTE	bAGC;
	short	iAGC_TMA;
	short	iAGC_AVCR;
	short	iAGC_DVCR;
	short	iAGC_MA;
	
	/* Activiry Detecting Level Audiocodes*/
	BYTE	bACTD;
	short	iACTD_ThLow;
	short	iACTD_ThHigh;
	short	iACTD_MinSil;
	short	iACTD_MaxSil;
	int		iACTD_MinActdMs;
	int		iACTD_MaxActdMs;

	/* Voice Detection Audiocodes*/
	BYTE	bVD;
	short	iVD_ThPower;
	short	iVD_RelTh;
	short	iVD_ThFilterCnt;
	short	iVD_NoVoiceMin;
	short	iVD_Reset;
	short	iVD_VoiceMin;
	short	iVD_McMin;
	/* Voltage Audiocodes */
	BYTE	bLV;
	short	iLV_Low;
	short	iLV_High;
	short	iLV_DeglitchTimeMs;

	/* Mobile Info */
	BYTE	iComPort;
	char	szSerDevName[32];
	short	iSpeed;
	char	szMobileHangupString[32];
	char	szInitString[32];
	char	szAnswerString[16];

	BYTE	bCheckSMS;

}T_AudioCodeChan;



typedef struct
{
	short	iSiteId;
	short	iCompanyId;
	BYTE	iRecServerId;

	
	short	i_channelid;
	BYTE	iRecordingMode; //i_channelmode;
	BYTE	iRecordingFormat; //i_recordingFormat;
	char	szTappingNum[32];
	char	szChannelName[32];

	BYTE	iStream;
	BYTE	bUseScheduling;
	unsigned int		iFromSchedulingTime;
	unsigned int		iToSchedulingTime;


	short	i_playGain;
	int		i_voxLevel;
	short	i_voxPresenseTime;
	short	iVOXAbsenceTime ;
	
	BYTE	i_AGCIsEnable;

	short	i_BargeInSense;
	short	i_RecordGain;
	short	iHookVoltage;



	/* Mobile Info */
	BYTE	iComPort;
	char	szSerDevName[32];
	short	iSpeed;
	char	szMobileHangupString[32];
	char	szInitString[32];
	char	szAnswerString[16];
	BYTE	bCheckSMS;


	BYTE	i_Used;
}T_SynwayChan;

typedef struct
{
	short iCompanyId;
	short iSiteId;
}T_MyId;

/* [13-11-2013]	[SG]	*/
enum
{
	TYPE_UNKNOWN,
	TYPE_RECORDING,
	TYPE_RECORDING_NOFILE,
	TYPE_MISSEDCALL,
	TYPE_SITE_STATUS
};

/* [27-06-2014]	[SG]	*/
#define PWD_WINDOWSLOGIN		"__WINDOWSVERIFIEDUSER__"

/* [01-11-2014]	[SG]	*/
typedef struct
{
	char szSource[128];
	char szDest[128];
	unsigned int iSize;

}T_FtpHeader;
#endif 
