//night crawler
void switch_IR_camera(bool IR_camera)
{
    if (IR_camera == 1)
    {
        // switch it on
        digitalWrite(, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);          // wait for a second
    }
    else if (IR_camera == 0)
    {
        digitalWrite(, LOW); // turn the LED off by making the voltage LOW
        delay(1000);         // wait for a second
    }
}