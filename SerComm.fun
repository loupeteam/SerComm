(********************************************************************
 * COPYRIGHT -- Automation Resources Group
 ********************************************************************
 * Library: SerComm
 * File: SerComm.fun
 * Author: David Blackburn
 * Created: June 21, 2013
 ********************************************************************
 * Functions and function blocks of library SerComm
 ********************************************************************)

FUNCTION SerialCommFn_Cyclic : UINT (*Cyclic function for managing serial communications*)
	VAR_IN_OUT
		t : SerialCommMgr_typ;
	END_VAR
END_FUNCTION

FUNCTION SerialCommMgr : UINT (*Cyclic function for managing serial communications*)
	VAR_IN_OUT
		t : SerialCommMgr_typ;
	END_VAR
END_FUNCTION

FUNCTION SerialCommSend : UINT (*Cyclic function for managing serial communications*)
	VAR_IN_OUT
		t : SerialCommMgr_typ;
	END_VAR
END_FUNCTION

FUNCTION SerialCommReceive : UINT (*Cyclic function for managing serial communications*)
	VAR_IN_OUT
		t : SerialCommMgr_typ;
	END_VAR
END_FUNCTION
