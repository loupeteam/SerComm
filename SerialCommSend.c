/*
 * File: SerialCommSend.c
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

/* Main state machine								DONE and TESTED DFB 20130711 */

	/* SERCOMM_ST_RUNNING							DONE and TESTED DFB 20130718 */
			
		/* Send Data								DONE and TESTED DFB 20130718 */

			/* SERCOMM_SEND_ST_GBUF					DONE and TESTED DFB 20130718 */
				/* Done								DONE and TESTED DFB 20130718 */
				/* Error							DONE will not be tested */

			/* SERCOMM_SEND_ST_IDLE					DONE and TESTED DFB 20130718 */
				/* CMD latching						DONE and TESTED DFB 20130718 */
				/* Data fits						DONE and TESTED DFB 20130718 */
				/* Data too big						DONE and TESTED DFB 20130718 */

			/* SERCOMM_SEND_ST_WRITE				DONE and TESTED DFB 20130718 */
				/* Done								DONE and TESTED DFB 20130718 */
				/* Error							DONE and TESTED DFB 20130718 */

			/* SERCOMM_SEND_ST_ROBUF				DONE and TESTED DFB 20130718 */
				/* Done								DONE and TESTED DFB 20130718 */
				/* Error							DONE and TESTED DFB 20130718 */
	
		/* Handle Enable							DONE and TESTED DFB 20130710 */
	
unsigned short SerialCommSend(struct SerialCommMgr_typ* t)
{


/************************************************/
/* Check input									*/
/************************************************/

if( t == 0 ){
	return (UINT)SERCOMM_ERR_INVALIDINPUT;
}

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

switch( t->Internal.CommState ){
		
	case SERCOMM_ST_RUNNING:
					
		/****************************************************/
		/* Send data 										*/
		/****************************************************/
		
		switch( t->Internal.SendState ){
		
		
			case SERCOMM_SEND_ST_GBUF:

				/* Wait for write buffer */
					
				if( t->Internal.FUB.GetWriteBuffer.status < ERR_FUB_ENABLE_FALSE ){
				
					t->Internal.FUB.GetWriteBuffer.enable=	0;
					
					
					if( t->Internal.FUB.GetWriteBuffer.status == 0 ){
					
						/* Have buffer. Grab info and wait for Send command. */
						
						t->Internal.WriteBuffer=		t->Internal.FUB.GetWriteBuffer.buffer;
						t->Internal.WriteBufferLength=	t->Internal.FUB.GetWriteBuffer.buflng;
						
						t->Internal.SendState=	SERCOMM_SEND_ST_IDLE;
						
					} // Good buffer //
					
					
					else{
					
						/* Error. Only way to fix it is to close and open frame driver? */
							
						t->OUT.STAT.Error=		1;
						t->OUT.STAT.ErrorID=	t->Internal.FUB.GetWriteBuffer.status;
						strncpy( t->OUT.STAT.ErrorString, "Error getting send buffer. Disable communications and check CFG values.", sizeof(t->OUT.STAT.ErrorString) - 1 );
						
					} // Error getting buffer //
					
					
				} // Done //
					
							
			case SERCOMM_SEND_ST_IDLE:
				
				/* Wait for a send command */
				
				if( t->IN.CMD.Send && !t->Internal.CMD.Send ){
					
					t->Internal.CMD.Send=	1;
				
					if( t->IN.PAR.DataLength <= t->Internal.WriteBufferLength ){
					
						memcpy( (void*)t->Internal.WriteBuffer, &(t->IN.PAR.Data), t->IN.PAR.DataLength);
					
						t->Internal.FUB.Write.enable=	1;
						t->Internal.FUB.Write.ident=	t->Internal.FrameIdent;
						t->Internal.FUB.Write.buffer=	t->Internal.WriteBuffer;
						t->Internal.FUB.Write.buflng=	t->IN.PAR.DataLength;
						FRM_write(&t->Internal.FUB.Write);

						t->Internal.SendState=	SERCOMM_SEND_ST_WRITE;
					
					} // Send data fits in buffer //
					
					else{
					
						t->OUT.STAT.Error=		1;
						t->OUT.STAT.ErrorID=	SERCOMM_ERR_SENDDATATOOLARGE;
						strncpy( t->OUT.STAT.ErrorString, "Send data is too large.", sizeof(t->OUT.STAT.ErrorString) - 1 );
						break;				
						
					} // Send data too large //
					
				} // Send //
				else{
					break;				
				}
			
				
				
			case SERCOMM_SEND_ST_WRITE:
			
				/* Write the data. If no error, go to ST_DONE. If error, go to ST_ROBUF. */
				
				if( t->Internal.FUB.Write.status < ERR_FUB_ENABLE_FALSE ){
				
					t->Internal.FUB.Write.enable=	0;
					
					
					if( t->Internal.FUB.Write.status == 0 ){
					
						/* All good. Get a new buffer */
						
						t->OUT.STAT.DataSent=	1;
						
						t->Internal.FUB.GetWriteBuffer.enable=	1;
						t->Internal.FUB.GetWriteBuffer.ident=	t->Internal.FrameIdent;
				
						t->Internal.SendState=	SERCOMM_SEND_ST_GBUF;
							
					} // Good //
					
					
					else{
					
						/* Error writing. Release write buffer. */
						
						t->OUT.STAT.Error=		1;
						t->OUT.STAT.ErrorID=	t->Internal.FUB.Write.status;
						strncpy( t->OUT.STAT.ErrorString, "Error sending data.", sizeof(t->OUT.STAT.ErrorString) - 1 );
						
						t->Internal.FUB.ReleaseWriteBuffer.enable=	1;
						t->Internal.FUB.ReleaseWriteBuffer.ident=	t->Internal.FrameIdent;
						t->Internal.FUB.ReleaseWriteBuffer.buffer=	t->Internal.WriteBuffer;
						t->Internal.FUB.ReleaseWriteBuffer.buflng=	t->Internal.WriteBufferLength;
						
						t->Internal.SendState=	SERCOMM_SEND_ST_ROBUF;

					} // Error //	
						
				} // Done //
				
				break;						

			case SERCOMM_SEND_ST_ROBUF:
				
				/* Wait for output buffer to be released  */
				
				if( t->Internal.FUB.ReleaseWriteBuffer.status < ERR_FUB_ENABLE_FALSE ){
					
					t->Internal.FUB.ReleaseWriteBuffer.enable=	0;
					
					
					if( t->Internal.FUB.ReleaseWriteBuffer.status == 0 ){
					
						/* Done. Get a new buffer */
						
						t->Internal.FUB.GetWriteBuffer.enable=	1;
						t->Internal.FUB.GetWriteBuffer.ident=	t->Internal.FrameIdent;
				
						t->Internal.SendState=	SERCOMM_SEND_ST_GBUF;
						
					} // Good //
					
					
					else{
					
						/* Error. Probably fatal. Wait for Enable to go false,
							then recover normally. */
							
						t->OUT.STAT.Error=		1;
						t->OUT.STAT.ErrorID=	t->Internal.FUB.ReleaseWriteBuffer.status;
						strncpy( t->OUT.STAT.ErrorString, "Error releasing send buffer. Disable communications and check CFG values.", sizeof(t->OUT.STAT.ErrorString) - 1 );
						
					} // Error //
					
					
				} // Done //	
				
				break;
				
			
		} // switch(SendState) //
				
		break;
		
	case SERCOMM_ST_CLOSING:
	case SERCOMM_ST_CLOSED:
	case SERCOMM_ST_OPENING:		
	case SERCOMM_ST_ERROR:
			break;		
} // switch(CommState) //


/* Call FUBs */

FRM_gbuf(&t->Internal.FUB.GetWriteBuffer);
FRM_write(&t->Internal.FUB.Write);
FRM_robuf(&t->Internal.FUB.ReleaseWriteBuffer);

return 0;

} // End Fn //
