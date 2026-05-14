1.0.1   - Fix broken v1.0.0 publish: register SerComm LibraryObject in Cpu.sw so the publish workflow exports the `.br` runtime; also repair ARM Cpu.sw empty `<Libraries />` (predated librarybuilderproject@1.0.1 scaffold fix).

1.0.0   - AS6 migration.

0.05.1  - Removed some warnings
0.05.0  - Split read/write/manage into separate functions.
0.04.0	- Successful read/writes do not "break" allowing reads/writes every cycle.
	   	- Added AllowContinuousSend/AllowContinuousReceive to read/write every cycle.	
0.03.0 	- Added documentation.

0.02.0 	- First version.
