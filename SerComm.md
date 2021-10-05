![Automation Resources Group](http://automationresourcesgroup.com/images/arglogo254x54.png)

#SerComm Library
The SerComm library is an easy to use, fast, and reliable solution for serial communications.

#Usage

##Initialization
To use the SerComm functionality, a variable must be declared of type **SerialCommMgr_typ**. The SerialCommMgr uses meaningful default values for configuration settings, but these can be changed if necessary.
 

##Cyclic Operation
The **SerialCommFn_Cyclic()** function must be called in the CYCLIC routine of your program once every scan, unconditionally.

	SerialCommFn_Cyclic(SerialComm);

High Throughput Read/Write

	//Call manager to open/close
	SerialCommMgr(SerialComm);
	
	//Get any data from the driver
	SerialCommReceive(SerialComm);

	//Do some logic here

	//Echo what we received
	IF SerialComm.OUT.STAT.NewDataAvailable THEN
		memcpy(ADR(SerialComm.IN.PAR.Data), ADR(SerialComm.OUT.STAT.Data), SerialComm.OUT.STAT.DataLength);
		SerialComm.IN.PAR.DataLength:=	SerialComm.OUT.STAT.DataLength;
		SerialComm.IN.CMD.Send:=		TRUE;
	END_IF

	//Send any data out
	SerialCommSend(SerialComm);

	SerialComm.IN.CMD.Send:=		FALSE;

The communications are enabled by setting **IN.CMD.Enable** to TRUE. Once communications are established, **OUT.STAT.CommunicationActive** is set to TRUE and it is possible to send and receive data.

	SerialCommMgr.IN.CMD.Enable:=	1;

To send data, the data must first be copied into the **IN.PAR.Data** array and **IN.PAR.DataLength** must be set to the number of bytes you wish to send. Then **IN.CMD.Send** must be set to TRUE. Once the data is sent, **OUT.STAT.DataSent** will be set to TRUE and will remain TRUE until **IN.CMD.Send** is reset to FALSE. **IN.PAR.Data** and **IN.PAR.DataLength** are not altered in any way by the **SerialCommFn_Cyclic()** function. So, it is recommended to clear the **IN.PAR.Data** array before copying data into it. Also, be careful to ensure that **IN.PAR.DataLength** is set appropriately.

	IF( Send )THEN

		Send:=	0;

		memset( ADR(SerialCommMgr.IN.PAR.Data), 0, SIZEOF(SerialCommMgr.IN.PAR.Data) );
		strcpy( ADR(SerialCommMgr.IN.PAR.Data), ADR('POLL REQUEST') );
		SerialCommMgr.IN.PAR.DataLength:=	strlen( ADR(SerialCommMgr.IN.PAR.Data) );

		SerialCommMgr.IN.CMD.Send:=	1;

	END_IF
	
	IF( SerialCommMgr.OUT.STAT.DataSent )THEN
		SerialCommMgr.IN.CMD.Send:=	0;
	END_IF

Any data that is sent to the controller is received automatically. When new data is received, **OUT.STAT.NewDataAvailable** is set to TRUE, **OUT.STAT.DataLength** is set to the length of the received data, and the received data is copied into the **OUT.STAT.Data** array. The received data must be acknowledged by setting **IN.CMD.AcknowledgeData** to TRUE. This will set **OUT.STAT.NewDataAvailable** to FALSE and allow the SerialCommMgr to receive new data. When new data is received, **OUT.STAT.Data** is cleared before copying the new data into the array.

	IF( SerialCommMgr.OUT.STAT.NewDataAvailable )THEN

		memset( ADR(ResponseData), 0, SIZEOF(ResponseData) );
		memcpy( ADR(ResponseData), ADR(SerialCommMgr.OUT.STAT.Data), SerialCommMgr.OUT.STAT.DataLength );

		(* Do something with ResponseData *)

		SerialCommMgr.IN.CMD.AcknowledgeData:=	1;

	END_IF
	
	

Error information is available in **OUT.STAT.Error**, **OUT.STAT.ErrorID**, and **OUT.STAT.ErrorString**. Error information is cleared by setting **IN.CMD.AcknowledgeError** to TRUE. Some errors require toggling **IN.CMD.Enable** to restore communications.

#Reference

##SerialCommMgr Data Structure
The main data structure of the SerComm library is the SerialCommMgr structure (SerialCommMgr_typ datatype). This provides the interface to higher level programs and also stores all necessary internal information. It is divided into inputs (SerialCommMgr.IN), outputs (SerialCommMgr.OUT), and internals (SerialCommMgr.Internal).

###Inputs
The SerialCommMgr inputs are divided into commands (IN.CMD), parameters (IN.PAR), and configuration settings (IN.CFG). Commands are used to initiate operations, and parameters and configuration settings determine how the commands will be processed. The difference between parameters and configuration settings is that configuration settings are generally set only once, while parameters might be set any time a command is issued.

####Commands
* **Enable** - Enable the communications driver.
* **Send** - Send data. Communications must be enabled before sending data.
* **AcknowledgeData** - Acknowledge received data. Received data must be acknowledge before new data can be received.
* **AcknowledgeError** - Acknowledge any errors.

####Parameters
* **Data** - Data to be sent.
* **DataLength** - Length of the data that should be sent.
* **AllowContinuousSend** - Allows sending every cycle without seeing a FALSE on the send command.
* **AllowContinuousReceive** - Allows receiving every cycle without seeing an AcknowledgeData command. Data will be overwritten if new data arrives.

####Configuration Settings
* **INADeviceName** - INA device name of the serial interface to use. The default value is 'IF1'.
* **Mode** - Mode string. The default value is an empty string. This uses the settings defined for the interface in Automation Studio. For details on the mode string, please see the AS online help for the DVFrame library. Please be sure to use the SG4 format and not the SG3 format.
* **pConfig** - Address of an extended configuration data object of type XOPENCONFIG. For details, please see the AS online help for the DVFrame library.


###Outputs
The SerialCommMgr outputs contain status information (OUT.STAT), including received data.

* **CommunicationActive** - Communication is active. Data can be sent and received.
* **DataSent** - Data has been sent. This will remain TRUE until **IN.CMD.Send** is reset to FALSE.
* **NewDataAvailable** - New data has been received and is available in **OUT.STAT.Data**. This will remain TRUE until **IN.CMD.AcknowledgeData** is set to TRUE. Received data must be acknowledged before new data can be received.
* **Data** - Received data. This is cleared before copying newly received data.
* **DataLength** - Length of the data received.
* **Error** - An error is present. **Error** is reset by **IN.CMD.AcknowledgeError**. 
* **ErrorID** - Current error ID number.
* **ErrorString** - Current error text.

##Error ID Numbers
* 60, 8071-8073, 8078, 8079, 8210, 8251-8258 - These errors are passed along from the internal DVFrame function blocks. These errors are most likely caused by improper configuration information. For details, please see the AS online help for the DVFrame library.
* 50000 - SERCOMM_ERR_INVALIDINPUT - The **SerialCommMgrFn_Cyclic()** function was called with an invalid input. Check the function call.
* 50001 - SERCOMM_ERR_RECVDATATOOLARGE - The data received did not fit in the **OUT.STAT.Data** array. Only 256 bytes can be received at one time. The first 256 bytes are copied into **OUT.STAT.Data** and **OUT.STAT.DataLength** is set to 256. The remaining bytes are ignored.
* 50002 - SERCOMM_ERR_SENDDATATOOLARGE - **IN.PAR.DataLength** is set higher than 256, the maximum number of bytes that can be sent.

##Changing the Maximum DataLength
By default, the maximum length of data that can be sent or received at one time is 256 bytes. If this is not enough data for your application, then **SERCOMM_MAI_DATA** in the SerComm.var file must be changed. It should be set to one less than the maximum desired data length (e.g. 511 for 512 bytes of data). This also changes the threshold for triggering DATATOOLARGE errors. The library must be recompiled and transfered to the target for this change to take effect. Also, the XOPENCONFIG structure must be used to change the length of the transmit and receive frames used by the frame driver. For details on this, please see the AS online help for the DVFrame library.