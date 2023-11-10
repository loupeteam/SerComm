/*
 * File: SerialCommMgr.c
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of SerComm, licensed under the MIT License.
 *
 */

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif

	#include "SerComm.h"

#ifdef __cplusplus
	};
#endif


#include <string.h>


/********************************************************************/
/* Cyclic function for managing serial communications 				*/
/********************************************************************/

/* Check input										DONE and TESTED DFB 20130710 */

/* Handle AcknowledgeData							DONE and TESTED DFB 20130710 */

/* Reset Internal.CMD.Send and DataSent				DONE and TESTED DFB 20130710 */

/* Main state machine								DONE and TESTED DFB 20130711 */

	/* SERCOMM_ST_CLOSED							DONE and TESTED DFB 20130710 */
		/* Reset FUB enables						DONE and TESTED DFB 20130710 */
		/* Reset STAT								DONE and TESTED DFB 20130710 */
		/* Handle Enable							DONE and TESTED DFB 20130710 */
		
	/* SERCOMM_ST_OPENING							DONE and TESTED DFB 20130710 */
		/* Open good								DONE and TESTED DFB 20130710 */
		/* Error opening							DONE and TESTED DFB 20130710 */
		
	/* SERCOMM_ST_RUNNING							DONE and TESTED DFB 20130718 */

		/* Set CommunicationActive					DONE and TESTED DFB 20130710 */
	
	/* SERCOMM_ST_CLOSING							DONE and TESTED DFB 20130711 */
		/* Done										DONE and TESTED DFB 20130710 */
		/* Not opened								DONE and TESTED DFB 20130710 */
		/* Error									DONE will not be tested */
		
	/* SERCOMM_ST_ERROR								DONE and TESTED DFB 20130710 */

/* Handle AcknowledgeError							DONE and TESTED DFB 20130710 */

	
unsigned short SerialCommMgr(struct SerialCommMgr_typ* t)
{


/************************************************/
/* Check input									*/
/************************************************/

if( t == 0 ){
	return (UINT)SERCOMM_ERR_INVALIDINPUT;
}


/************************************************/
/* Handle AcknowledgeData 						*/
/************************************************/

if( t->IN.CMD.AcknowledgeData ){
	t->OUT.STAT.NewDataAvailable=	0;
}

t->IN.CMD.AcknowledgeData=	0;
	

/************************************************/
/* Reset internal Send CMD and DataSent			*/
/************************************************/

if( !t->IN.CMD.Send || t->IN.PAR.AllowContinuousSend ){
	t->Internal.CMD.Send=	0;
	t->OUT.STAT.DataSent=	0;
}


/************************************************/
/* Main state machine							*/
/************************************************/

/* Default CommunicationActive to 0 */

t->OUT.STAT.CommunicationActive=	0;


switch( t->Internal.CommState ){


	case SERCOMM_ST_CLOSED:
	
		/* Reset FUB enables */
		
		t->Internal.FUB.Open.enable=	0;

		t->Internal.FUB.Read.enable=	0;
		t->Internal.FUB.ReleaseReadBuffer.enable=	0;
				
		t->Internal.FUB.GetWriteBuffer.enable=	0;
		t->Internal.FUB.Write.enable=	0;
		t->Internal.FUB.ReleaseWriteBuffer.enable=	0;

		t->Internal.FUB.Close.enable=	0;
		
		
		/* Reset status */
		
		memset( &(t->OUT.STAT), 0, sizeof(t->OUT.STAT) );
		
		
		/* Wait for Enable and then open frame */
		
		if( t->IN.CMD.Enable ){
		
			t->Internal.FUB.Open.enable=	1;
			t->Internal.FUB.Open.device=	(UDINT)&(t->IN.CFG.INADeviceName);
			t->Internal.FUB.Open.mode=		(UDINT)&(t->IN.CFG.Mode);
			t->Internal.FUB.Open.config=	t->IN.CFG.pConfig;
				
			t->Internal.CommState=	SERCOMM_ST_OPENING;
			
		}
		
		break;
		
	
	case SERCOMM_ST_OPENING:
	
		/* Wait for frame to be open and then go to ST_RUNNING */
		
		if( t->Internal.FUB.Open.status < ERR_FUB_ENABLE_FALSE ){
		
			t->Internal.FUB.Open.enable=	0;
		
			
			if( t->Internal.FUB.Open.status == 0 ){
			
				/* All good, grab ident, reset state machines and move on */
				
				t->Internal.FrameIdent=		t->Internal.FUB.Open.ident;
				
				t->Internal.CommState=		SERCOMM_ST_RUNNING;
				
				
				t->Internal.FUB.Read.enable=	1;
				t->Internal.FUB.Read.ident=		t->Internal.FrameIdent;
				
				t->Internal.ReceiveState=	SERCOMM_RECV_ST_READ;
				
				
				t->Internal.FUB.GetWriteBuffer.enable=	1;
				t->Internal.FUB.GetWriteBuffer.ident=	t->Internal.FrameIdent;
				
				t->Internal.SendState=		SERCOMM_SEND_ST_GBUF;
				
			} // Good //
			
			
			else{
			
				/* Error. Probably fatal.  Wait for Enable to go false then recover. */
				
				t->OUT.STAT.Error=		1;
				t->OUT.STAT.ErrorID=	t->Internal.FUB.Open.status;
				strncpy( t->OUT.STAT.ErrorString, "Error opening frame driver. Check CFG values.", sizeof(t->OUT.STAT.ErrorString) - 1 );
				
				t->Internal.CommState=	SERCOMM_ST_ERROR;
				
			} // Error //
			
			
		} // Done //
		
		break;
		
		
	case SERCOMM_ST_RUNNING:
	
		
		/* Comms are active. Handle sending and receiving data in parallel.
			If !Enable, close frame and go to ST_CLOSING. */
			
		t->OUT.STAT.CommunicationActive=	1;
						
		/* Check for !Enable */
		/* Do I care about the other FUB statuses before disabling them?
			Not if Close really kills everything. */
		
		if( !(t->IN.CMD.Enable) ){
		
		
			/* Reset FUB enables */
			
			t->Internal.FUB.Open.enable=	0;
	
			t->Internal.FUB.Read.enable=	0;
			t->Internal.FUB.ReleaseReadBuffer.enable=	0;
					
			t->Internal.FUB.GetWriteBuffer.enable=	0;
			t->Internal.FUB.Write.enable=	0;
			t->Internal.FUB.ReleaseWriteBuffer.enable=	0;
	
			t->Internal.FUB.Close.enable=	0;
			
			
			/* Close frame */
			
			t->Internal.FUB.Close.enable=	1;
			t->Internal.FUB.Close.ident=	t->Internal.FrameIdent;
			
			t->Internal.CommState=	SERCOMM_ST_CLOSING;
			
			
		} // Disable comms //
		
		break;
		
	
	case SERCOMM_ST_CLOSING:
	
		/* Wait for frame to close and go to ST_CLOSED */
		
		if( t->Internal.FUB.Close.status < ERR_FUB_ENABLE_FALSE ){
		
			t->Internal.FUB.Close.enable=	0;

			t->Internal.FrameIdent=	0;
			
			if( 	(t->Internal.FUB.Close.status != 0)
				&&	(t->Internal.FUB.Close.status != frmERR_NOTOPENED)
				){
			
				/* Error. Weird. This error will be hard to see, since errors are cleared when you 
					get back to SERCOMM_ST_CLOSED. */
					
				t->OUT.STAT.Error=		1;
				t->OUT.STAT.ErrorID=	t->Internal.FUB.Close.status;
				strncpy( t->OUT.STAT.ErrorString, "Error closing frame driver.", sizeof(t->OUT.STAT.ErrorString) - 1 );
				
			} // Error //
			
			t->Internal.CommState=	SERCOMM_ST_CLOSED;			
			
		} // Done //
	
		break;
		

	case SERCOMM_ST_ERROR:
	
		/* Reset FUB enables */
		
		t->Internal.FUB.Open.enable=	0;

		t->Internal.FUB.Read.enable=	0;
		t->Internal.FUB.ReleaseReadBuffer.enable=	0;
				
		t->Internal.FUB.GetWriteBuffer.enable=	0;
		t->Internal.FUB.Write.enable=	0;
		t->Internal.FUB.ReleaseWriteBuffer.enable=	0;

		t->Internal.FUB.Close.enable=	0;
		
		
		/* Wait for Enable to be FALSE */
	
		if( !(t->IN.CMD.Enable) ){
		
			t->Internal.CommState=	SERCOMM_ST_CLOSED;
			
		}
		
		break;
		

} // switch(CommState) //


/* Call FUBs */

FRM_xopen(&t->Internal.FUB.Open);

FRM_close(&t->Internal.FUB.Close);


/* Handle AcknowledgeError */

if( t->IN.CMD.AcknowledgeError ){
	t->OUT.STAT.Error=		0;
	t->OUT.STAT.ErrorID=	0;
	strcpy(t->OUT.STAT.ErrorString, "");
}

t->IN.CMD.AcknowledgeError=	0;


return 0;


} // End Fn //
