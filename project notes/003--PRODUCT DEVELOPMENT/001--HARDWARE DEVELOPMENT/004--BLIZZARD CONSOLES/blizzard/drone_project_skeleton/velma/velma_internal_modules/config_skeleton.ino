//this is the arduino file, it enables the software to work on arduino because it holds all the dependencies, 
//and functionalities not yet perfectly places, like timers, 
//this also holds configuration files for the modules,  
/*can i have configuration files in a library, all dependencies of the arduino board, setup files, pin configurations will be done on this file, 
such that the rest of the code is abstracted enough to run on a different drone with a different board
thus to change the pins I do it in this library, yes?
how to handle each board and module, code is here, this will be easier, the code e.g. from flight stabailization receives data in deg/s, 
and this is verifiable via tests, the code that reads the MPU doesn't know of any flight stabilization software, 

every other library that uses a physical board uses config_skeleton.ino or configuration file.... 
the code that begins these boards, setup file. this will allow code to run even on an arduino uno, 
ultrasonic sensor, etc, 
this file joins the software to the hardware, with this everything else happens, 

this helps read the modules, with this file, the software knows how to communicate with hardware

it connects to arduino uno, arduino nano, arduino mega, etc, runs device tests, knows which modules are available or not

*/ 
