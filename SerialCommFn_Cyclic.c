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

	
unsigned short SerialCommFn_Cyclic(struct SerialCommMgr_typ* t)
{

	SerialCommMgr(t);
	SerialCommReceive(t);
	SerialCommSend(t);
	
	return 0;
} // End Fn //
