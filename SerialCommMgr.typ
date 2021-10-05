(********************************************************************
 * COPYRIGHT -- Automation Resources Group
 ********************************************************************
 * Library: SerComm
 * File: SerialCommMgr.typ
 * Author: David Blackburn
 * Created: June 21, 2013
 ********************************************************************
 * Data types of library SerComm
 ********************************************************************)

TYPE
	SERCOMM_ERR_enum : 
		(
		SERCOMM_ERR_INVALIDINPUT := 50000,
		SERCOMM_ERR_RECVDATATOOLARGE,
		SERCOMM_ERR_SENDDATATOOLARGE,
		SERCOMM_ERR_
		);
	SERCOMM_ST_enum : 
		(
		SERCOMM_ST_CLOSED,
		SERCOMM_ST_OPENING,
		SERCOMM_ST_RUNNING,
		SERCOMM_ST_CLOSING,
		SERCOMM_ST_ERROR
		);
	SERCOMM_RECV_ST_enum : 
		(
		SERCOMM_RECV_ST_READ,
		SERCOMM_RECV_ST_RBUF
		);
	SERCOMM_SEND_ST_enum : 
		(
		SERCOMM_SEND_ST_GBUF,
		SERCOMM_SEND_ST_IDLE,
		SERCOMM_SEND_ST_WRITE,
		SERCOMM_SEND_ST_ROBUF
		);
	SerialCommMgr_Int_FUB_typ : 	STRUCT 
		Open : FRM_xopen;
		Read : FRM_read;
		ReleaseReadBuffer : FRM_rbuf;
		GetWriteBuffer : FRM_gbuf;
		Write : FRM_write;
		ReleaseWriteBuffer : FRM_robuf;
		Close : FRM_close;
	END_STRUCT;
	SerialCommMgr_Internal_typ : 	STRUCT 
		CommState : SERCOMM_ST_enum;
		SendState : SERCOMM_SEND_ST_enum;
		ReceiveState : SERCOMM_RECV_ST_enum;
		FrameIdent : UDINT;
		WriteBuffer : UDINT;
		WriteBufferLength : UINT;
		CMD : SerialCommMgr_IN_CMD_typ;
		FUB : SerialCommMgr_Int_FUB_typ;
	END_STRUCT;
	SerialCommMgr_OUT_STAT_typ : 	STRUCT 
		CommunicationActive : BOOL;
		DataSent : BOOL;
		NewDataAvailable : BOOL;
		Data : ARRAY[0..SERCOMM_MAI_DATA]OF USINT;
		DataLength : UDINT;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[SERCOMM_STRLEN_ERRORSTRING];
	END_STRUCT;
	SerialCommMgr_OUT_typ : 	STRUCT 
		STAT : SerialCommMgr_OUT_STAT_typ;
	END_STRUCT;
	SerialCommMgr_IN_CFG_typ : 	STRUCT 
		INADeviceName : STRING[SERCOMM_STRLEN_DEVICENAME] := 'IF1';
		Mode : STRING[SERCOMM_STRLEN_MODE];
		pConfig : UDINT;
	END_STRUCT;
	SerialCommMgr_IN_PAR_typ : 	STRUCT 
		Data : ARRAY[0..SERCOMM_MAI_DATA]OF USINT;
		DataLength : UDINT;
		AllowContinuousSend : BOOL;
		AllowContinuousReceive : BOOL;
	END_STRUCT;
	SerialCommMgr_IN_CMD_typ : 	STRUCT 
		Enable : BOOL;
		Send : BOOL;
		AcknowledgeData : BOOL;
		AcknowledgeError : BOOL;
	END_STRUCT;
	SerialCommMgr_IN_typ : 	STRUCT 
		CMD : SerialCommMgr_IN_CMD_typ;
		PAR : SerialCommMgr_IN_PAR_typ;
		CFG : SerialCommMgr_IN_CFG_typ;
	END_STRUCT;
	SerialCommMgr_typ : 	STRUCT 
		IN : SerialCommMgr_IN_typ;
		OUT : SerialCommMgr_OUT_typ;
		Internal : SerialCommMgr_Internal_typ;
	END_STRUCT;
END_TYPE
