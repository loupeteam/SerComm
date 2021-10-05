/********************************************************************
 * COPYRIGHT -- Automation Resources Group
 ********************************************************************
 * Library: SerComm
 * File: SerialCommFn_Cyclic.c
 * Author: David Blackburn
 * Created: June 21, 2013
 ********************************************************************
 * Implementation of library SerComm
 ********************************************************************/

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

		/* Receive Data								DONE and TESTED DFB 20130711 */

			/* SERCOMM_RECV_ST_READ					DONE and TESTED DFB 20130710 */
				/* FUB enable						DONE and TESTED DFB 20130710 */
				/* FUB ident						DONE and TESTED DFB 20130710 */
				/* Good data						DONE and TESTED DFB 20130710 */
				/* Bad data							DONE and TESTED DFB 20130710 */
				/* No data							DONE and TESTED DFB 20130710 */
				/* Error							DONE and TESTED DFB 20130710 */

			/* SERCOMM_RECV_ST_RBUF					DONE and TESTED DFB 20130711 */
				/* Done								DONE and TESTED DFB 20130710 */
					/* Data not ack'ed				DONE and TESTED DFB 20130710 */
					/* Data already ack'ed			DONE and TESTED DFB 20130710 */
				/* Error							DONE will not be tested */
			
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
	
	/* SERCOMM_ST_CLOSING							DONE and TESTED DFB 20130711 */
		/* Done										DONE and TESTED DFB 20130710 */
		/* Not opened								DONE and TESTED DFB 20130710 */
		/* Error									DONE will not be tested */
		
	/* SERCOMM_ST_ERROR								DONE and TESTED DFB 20130710 */

/* Handle AcknowledgeError							DONE and TESTED DFB 20130710 */

	
unsigned short SerialCommReceive(struct SerialCommMgr_typ* t)
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
/* Main state machine							*/
/************************************************/

switch( t->Internal.CommState ){
		
	case SERCOMM_ST_RUNNING:
	
		/****************************************************/
		/* Receive data 									*/
		/****************************************************/
		
		switch( t->Internal.ReceiveState ){
		
		
			case SERCOMM_RECV_ST_READ:
			
				t->Internal.FUB.Read.enable=	!t->OUT.STAT.NewDataAvailable || t->IN.PAR.AllowContinuousReceive;
				t->Internal.FUB.Read.ident=		t->Internal.FrameIdent;
				
				if( t->Internal.FUB.Read.status < ERR_FUB_ENABLE_FALSE ){
				
				
					/* New data */

					if( t->Internal.FUB.Read.status == 0 ){
					
					
						/* Reset FUB */
							
						t->Internal.FUB.Read.enable=	0;
						
						
						/* Grab data */
						
						memset( &t->OUT.STAT.Data, 0, sizeof(t->OUT.STAT.Data) );
						
						if( t->Internal.FUB.Read.buflng <= sizeof(t->OUT.STAT.Data) ){

							memcpy( &t->OUT.STAT.Data, (void*)t->Internal.FUB.Read.buffer, t->Internal.FUB.Read.buflng );
							t->OUT.STAT.DataLength=	t->Internal.FUB.Read.buflng;
						
						} // Data fits //
						
						else{
						
							t->OUT.STAT.Error=		1;
							t->OUT.STAT.ErrorID=	SERCOMM_ERR_RECVDATATOOLARGE;
							strncpy( t->OUT.STAT.ErrorString, "Received data was too large.", sizeof(t->OUT.STAT.ErrorString) - 1 );
							
							memcpy( &t->OUT.STAT.Data, (void*)t->Internal.FUB.Read.buffer, sizeof(t->OUT.STAT.Data) );
							t->OUT.STAT.DataLength=	sizeof(t->OUT.STAT.Data);
							
						} // Data is too big //
						
						t->OUT.STAT.NewDataAvailable=	1;
						
						
						/* Release the buffer */
		
						t->Internal.FUB.ReleaseReadBuffer.enable=	1;
						t->Internal.FUB.ReleaseReadBuffer.ident=	t->Internal.FrameIdent;
						t->Internal.FUB.ReleaseReadBuffer.buffer=	t->Internal.FUB.Read.buffer;
						t->Internal.FUB.ReleaseReadBuffer.buflng=	t->Internal.FUB.Read.buflng;
						FRM_rbuf(&t->Internal.FUB.ReleaseReadBuffer);
			
						t->Internal.ReceiveState=	SERCOMM_RECV_ST_RBUF;
						
						
					} // New data //
					
					
					else if( t->Internal.FUB.Read.status == frmERR_NOINPUT ){

						break;
					
						/* No data. Keep looking. */
						
					} // No data //
					
					
					else{
					
						/* Error. Probably fatal. Keep going. Maybe you will get lucky. */
						
						t->Internal.FUB.Read.enable=	0;	
							
						t->OUT.STAT.Error=		1;
						t->OUT.STAT.ErrorID=	t->Internal.FUB.Read.status;
						strncpy( t->OUT.STAT.ErrorString, "Error reading data. Disable communications and check CFG values.", sizeof(t->OUT.STAT.ErrorString) - 1 );
						break;
						
					} // Fatal error //
					
					
				} // Done //				
			else{
				break;
			}
			case SERCOMM_RECV_ST_RBUF:
				
				/* Wait for buffer to be released */
				
				if( t->Internal.FUB.ReleaseReadBuffer.status < ERR_FUB_ENABLE_FALSE ){
				
					t->Internal.FUB.ReleaseReadBuffer.enable=	0;
					
					
					if( t->Internal.FUB.ReleaseReadBuffer.status == 0 ){
					
						t->Internal.FUB.Read.enable=	!t->OUT.STAT.NewDataAvailable || t->IN.PAR.AllowContinuousReceive;;
						t->Internal.FUB.Read.ident=		t->Internal.FrameIdent;
					
						t->Internal.ReceiveState=	SERCOMM_RECV_ST_READ;
						
					} // Good //
					
					
					else{
					
						/* Error. Probably fatal. Wait for disable comms. */
							
						t->OUT.STAT.Error=		1;
						t->OUT.STAT.ErrorID=	t->Internal.FUB.ReleaseReadBuffer.status;
						strncpy( t->OUT.STAT.ErrorString, "Error releasing receive buffer. Disable communications and check CFG values.", sizeof(t->OUT.STAT.ErrorString) - 1 );
						
					} // Error //	
						
				
				} // Done //
				
				break;

								
		} // switch(ReceiveState) //
				
		break;
		
	case SERCOMM_ST_CLOSING:
	case SERCOMM_ST_CLOSED:
	case SERCOMM_ST_OPENING:		
	case SERCOMM_ST_ERROR:
		break;		
} // switch(CommState) //


/* Call FUBs */

FRM_read(&t->Internal.FUB.Read);
FRM_rbuf(&t->Internal.FUB.ReleaseReadBuffer);

return 0;


} // End Fn //
