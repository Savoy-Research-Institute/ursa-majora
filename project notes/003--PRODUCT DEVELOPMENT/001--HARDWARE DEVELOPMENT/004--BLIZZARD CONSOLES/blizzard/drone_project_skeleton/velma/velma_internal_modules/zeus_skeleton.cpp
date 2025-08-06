/*this is the main file for cheatcodes, it runs both on the blizzard and the cube, has tests for both, 
//this is the cheatcode library it contains the cheat code execution scripts, 

each cheatcode must have a toggle and a variable variable, like a strength, 
on blizzard, when cheatcode button is pressed, cheatcode mode on,
                        if button if pressed again, set cheatcode mode off
shepherd, this allows the blizzard to search and rescue the drone, and at close proximity, both beep childconsole adaptive feedback,
        shepherd mode uses GPS data used to locate during a crash, magnetometer data from the blizzard and cube to calculate distance and directional data, 
        this is flushed to the screen, and leads the user for retrieval, calculates an error positive or negative, this is compared to the blizzard stance, giving an error
        onboard flashlights and LEDs help locate at night, 
cruise, this allows autopiloting capabilities, the drone maintains absolute altitude, pitch roll, speed, heading,
        this is also toggled temporarily when the cheatcode button is pressed and the user wants to input a cheat, the drone continues in flight for the seconds


cleopatra, the drone pitch channels data is influenced by a factor, by blizzard accelerometer and gyroscope data, 
        the drone will pitch up if the blizzard is pitched up, etc, 
        the blizzard will need to calibrate,  the user sits comfortably, that will be upright, tilting forward from that position is considered tilt #12344
    an alternative is to have the goggles control the camera on the gimbal, or itself to control drone flight
Osiris,^^^

pussypower, one of a kind stabilization software, the drone stabilizes, also influenced by a factor, the way a cat always knows where down is, the drone does, 
        it has an accelerometer,  which measures acceleration due to gravity,{ 9.8m/s" aka 1G }gravity always points down,
        the drone has a pull created in software, or a bias that always brings the channel values to 1500, i dont know how to impliment this correctly just yet, 

nightcrawler, this toggles IR LEDS all round the camera, simulating light amplification night vision, 
        this is useful at night, might automatically be toggled when photoresistor says its night time
        
sticklikeButter/Earthmagnet, this locks absolute altitude or height above ground, if implimented correctly, them drifts bruv...


Turtle_mode


drugger, the drone power is controlled by e.g. the digital volume, or the throttle, and the throttle pedal...
        dynamic throttle control, 
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
there are cheats that run while docked, or not, or any, 
if any logic error, print, cannot do as specified, cheatcode for docked mode, and simply not do it, 
if the code acts differently both times, like cube using peripheral LEDS, it knows if its docked or not,  wont run all the code
*/    
    /*
    read_hc12();
    if LB cheat button if pressed, or switchA #never changes
        enterCheatCodeMode();
        checkCheatCode();
        handleCheatCodeInput();
    if LB pressed again ;
        exitCheatCodeMode();
    */
   // Cleopatra will rely on the following data, the blizzard console will read from a gyroscope on the blizzard, below is a copy of functions from the drone
