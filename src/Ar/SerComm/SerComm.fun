(*
 * File: SerComm.fun
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of SerComm, licensed under the MIT License.
 *
 *)

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
