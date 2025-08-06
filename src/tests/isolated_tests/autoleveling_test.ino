#include <Wire.h> //Include the Wire.h library so we can communicate with the gyro.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PID gain and limit settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float pid_p_gain_roll =0.7;               //Gain setting for the roll P-controller (1.3)
float pid_i_gain_roll = 0.08;              //Gain setting for the roll I-controller (0.3)
float pid_d_gain_roll = 8;                //Gain setting for the roll D-controller (15)///////////////////OOOOOOOOOOOOOOOOOOOOOOOOOOOOKKKKKKKKKKKKKKKKKKKKKKKKKKKK
int pid_max_roll = 400;                    //Maximum output of the PID-controller (+/-)

float pid_p_gain_pitch = pid_p_gain_roll;  //Gain setting for the pitch P-controller.
float pid_i_gain_pitch = pid_i_gain_roll;  //Gain setting for the pitch I-controller.
float pid_d_gain_pitch = pid_d_gain_roll;  //Gain setting for the pitch D-controller.
int pid_max_pitch = pid_max_roll;          //Maximum output of the PID-controller (+/-)

float pid_p_gain_yaw = 1.0;                //Gain setting for the pitch P-controller. //4.0
float pid_i_gain_yaw = 0.02;               //Gain setting for the pitch I-controller. //0.02
float pid_d_gain_yaw = 0;                //Gain setting for the pitch D-controller.
//set at 0 because yaw has props drag already
int pid_max_yaw = 400;                     //Maximum output of the PID-controller (+/-)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Declaring Variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte last_channel_1, last_channel_2, last_channel_3, last_channel_4;
int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
int counter_channel_1, counter_channel_2, counter_channel_3, counter_channel_4, loop_counter;
int esc_1, esc_2, esc_3, esc_4;
int throttle, battery_voltage;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;
unsigned long zero_timer, timer_1, timer_2, timer_3, timer_4, current_time, current_timer;
int cal_int, start;
unsigned long loop_timer;
double gyro_pitch, gyro_roll, gyro_yaw;
double gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
byte highByte, lowByte;
int AccErrorX,AccErrorY,AccErrorZ;
float pid_error_temp;
float pid_i_mem_roll, pid_roll_setpoint, gyro_roll_input, pid_output_roll, pid_last_roll_d_error;
float pid_i_mem_pitch, pid_pitch_setpoint, gyro_pitch_input, pid_output_pitch, pid_last_pitch_d_error;
float pid_i_mem_yaw, pid_yaw_setpoint, gyro_yaw_input, pid_output_yaw, pid_last_yaw_d_error;
double GyroX, GyroY, GyroZ;
double GyroErrorX, GyroErrorY, GyroErrorZ;
const int MPU = 0x68; // MPU6050 I2C address
float gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, previousTime;
int c = 0;
int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
int lcd_loop_counter;
float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setup routine
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
Serial.begin(19200);
 Wire.begin();//Start the I2C as master.
// Enable Pin Change Interrupt for PCINT0 pins 10,11,12,13 respectively
  PCICR |= (1 << PCIE0);  
  PCMSK0 |= (1 << PCINT4);  
  PCMSK0 |= (1 << PCINT5);  
  PCMSK0 |= (1 << PCINT6);  
  PCMSK0 |= (1 << PCINT7);  
  DDRH |= B00010000;//Set pins as output(H4,7)(H3,6)(E3,5)(G4,8)(H5,8,output LED)
  DDRH |= B00001000;
  DDRE |= B00001000;
  DDRG |= B00100000;
  DDRH |= B00100000;
  //Arduino (Atmega) pins default to inputs, so they don't need to be explicitly declared as inputs.
  
  //Use the led on the Arduino for startup indication
  digitalWrite(8,HIGH);                                       //Turn on the warning led.
  //delay(2000);                                                 //Wait 2 second befor continuing.
  setupMPUregisters();
  delay(250);                                                  //Give the gyro time to start.  
  calculate_IMU_error();
  //Let's take multiple gyro data samples so we can determine the average gyro offset (calibration).
  //Now that we have 2000 measures, we need to devide by 2000 to get the average gyro offset.
//**********************************************************************************************************************************************
  while(receiver_input_channel_3 < 990 || receiver_input_channel_3 > 1020 || receiver_input_channel_4 < 1400){
    //Wait until the receiver is active and the throtle is set to the lower position.
    start ++;                                        //While waiting increment start whith every loop.
    
    //ESCS ESCS ESCS
    //We don't want the esc's to be beeping annoyingly. So let's give them a 1000us puls while waiting for the receiver inputs.
    PORTH |= B00011000;//set pin 6, 7 high(//Set digital poort 4, 5, 6 and 7 high.)
    PORTE |= B00001000;//Pin5
    PORTG |= B00100000;//Pin4
    //PH,PE,PG ESCS FOR MEGA
    delayMicroseconds(1000);                         //Wait 1000us (We can use delayMicroseconds because the receiver interrupt routine is not used).
    PORTH &= B11100111;//set pin 6, 7 LOW(//Set digital poort 4, 5, 6 and 7 low.)
    PORTE &= B11110111;//Pin5
    PORTG &= B11011111;//Pin4
    delay(3);                                        //Wait 3 milliseconds before the next loop.
    if(start == 125){                                //Every 125 loops (500ms).
      digitalWrite(8, !digitalRead(8));            //Change the led status.
      start = 0;                                     //Start again at 0.
    }
  } 
  start = 0;                                                   //Set start back to 0.
  
  //Load the battery voltage to the battery_voltage variable.
  //65 is the voltage compensation for the diode.
  //12.6V equals ~5V @ Analog 0.
  //12.6V equals 1023 analogRead(0).
  //1260 / 1023 = 1.2317.
  //The variable battery_voltage holds 1050 if the battery voltage is 10.5V.
  battery_voltage = (analogRead(0) + 65) * 1.2317;
  //  battery_voltage =analogRead(0);
  //When everything is done, turn off the led.
  digitalWrite(8, LOW);                             //Turn off the led.
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Main program loop
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  //Let's get the current gyro data and scale it to degrees per second for the pid calculations.
  readGyro();
    //Gyro angle calculations
  //0.0000611 = 1 / (250Hz / 65.5)
  angle_pitch += GyroX * 1;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += GyroY * 1;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(GyroZ * (3.142 / 180));               //If the IMU has yawed transfer the roll angle to the pitch angel
  angle_roll -= angle_pitch * sin(GyroZ * (3.142 / 180));               //If the IMU has yawed transfer the pitch angle to the roll angel
  //the above is in radians. Take note

  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  //multiplied to get answer in degrees, asin function gives answer in radians
  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  angle_pitch_acc -= 0.0;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                               //Accelerometer calibration value for roll

  if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
  
  //To dampen the pitch and roll angles a complementary filter is used
  //angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  //angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
//*************************************************************************************************************************************************************************8
  angle_roll_output = (angle_roll_output * 0.8) + ((GyroX) * 0.2);            //Gyro pid input is deg/sec.
  angle_pitch_output = (angle_pitch_output * 0.8) + ((GyroY ) * 0.2);         //Gyro pid input is deg/sec.
  gyro_yaw_input = (gyro_yaw_input * 0.8) + ((GyroZ ) * 0.2);               //Gyro pid input is deg/sec.

  //For starting the motors: throttle low and yaw left (step 1).
  if(receiver_input_channel_3 < 1050 && receiver_input_channel_4 < 1050)start = 1;
  //When yaw stick is back in the center position start the motors (step 2).
  if(start == 1 && receiver_input_channel_3 < 1050 && receiver_input_channel_4 > 1450){
    start = 2;
    //Reset the pid controllers for a bumpless start.
    pid_i_mem_roll = 0;
    pid_last_roll_d_error = 0;
    pid_i_mem_pitch = 0;
    pid_last_pitch_d_error = 0;
    pid_i_mem_yaw = 0;
    pid_last_yaw_d_error = 0;
  }
  //Stopping the motors: throttle low and yaw right.
  if(start == 2 && receiver_input_channel_3 < 1050 && receiver_input_channel_4 > 1950)start = 0;
  
  //The PID set point in degrees per second is determined by the roll receiver input.
  //In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  //Reduce for faster roll rate
  pid_roll_setpoint = 0;
  //We need a little dead band of 16us for better results.
  if(receiver_input_channel_1 > 1508)pid_roll_setpoint = (receiver_input_channel_1 - 1508)/3.0;
  else if(receiver_input_channel_1 < 1492)pid_roll_setpoint = (receiver_input_channel_1 - 1492)/3.0;
  //Reduce the three for faster roll rates
  //The PID set point in degrees per second is determined by the pitch receiver input.
  //In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_pitch_setpoint = 0;
  //We need a little dead band of 16us for better results.
  if(receiver_input_channel_2 > 1508)pid_pitch_setpoint = (receiver_input_channel_2 - 1508)/3.0;
  else if(receiver_input_channel_2 < 1492)pid_pitch_setpoint = (receiver_input_channel_2 - 1492)/3.0;
  
  //The PID set point in degrees per second is determined by the yaw receiver input.
  //In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_yaw_setpoint = 0;
  //We need a little dead band of 16us for better results.
  if(receiver_input_channel_3 > 1050){ //Do not yaw when turning off the motors.
    if(receiver_input_channel_4 > 1508)pid_yaw_setpoint = (receiver_input_channel_4 - 1508)/3.0;
    else if(receiver_input_channel_4 < 1492)pid_yaw_setpoint = (receiver_input_channel_4 - 1492)/3.0;
  }
  //PID inputs are known. So we can calculate the pid output.
  calculate_pid();
  
  //The battery voltage is needed for compensation.
  //A complementary filter is used to reduce noise.
  //0.09853 = 0.08 * 1.2317.
  battery_voltage = battery_voltage * 0.92 + (analogRead(0) + 65) * 0.09853;
  
  //Turn on the led if battery voltage is to low.
  if(battery_voltage < 1050 && battery_voltage > 600)digitalWrite(8, HIGH);
  
  throttle = receiver_input_channel_3;                                      //We need the throttle signal as a base signal.
  
  if (start == 2){                                                          //The motors are started.
    if (throttle > 1800) throttle = 1800;                                   //We need some room to keep full control at full throttle.
    esc_1 = throttle - pid_output_pitch + pid_output_roll - pid_output_yaw; //Calculate the pulse for esc 1 (front-right - CCW)
    esc_2 = throttle + pid_output_pitch + pid_output_roll + pid_output_yaw; //Calculate the pulse for esc 2 (rear-right - CW)
    esc_3 = throttle + pid_output_pitch - pid_output_roll - pid_output_yaw; //Calculate the pulse for esc 3 (rear-left - CCW)
    esc_4 = throttle - pid_output_pitch - pid_output_roll + pid_output_yaw; //Calculate the pulse for esc 4 (front-left - CW)

    if (battery_voltage < 1240 && battery_voltage > 800){                   //Is the battery connected?
      esc_1 += esc_1 * ((1240 - battery_voltage)/(float)3000);              //Compensate the esc-1 pulse for voltage drop.
      esc_2 += esc_2 * ((1240 - battery_voltage)/(float)3000);              //Compensate the esc-2 pulse for voltage drop.
      esc_3 += esc_3 * ((1240 - battery_voltage)/(float)3000);              //Compensate the esc-3 pulse for voltage drop.
      esc_4 += esc_4 * ((1240 - battery_voltage)/(float)3000);              //Compensate the esc-4 pulse for voltage drop.
    } //increase if drone rises
    
    if (esc_1 < 1100) esc_1 = 1050;                                         //Keep the motors running.
    if (esc_2 < 1100) esc_2 = 1050;                                         //Keep the motors running.
    if (esc_3 < 1100) esc_3 = 1050;       //all were @1200 at one point     //Keep the motors running.
    if (esc_4 < 1100) esc_4 = 1050;                                         //Keep the motors running.
    
    if(esc_1 > 2000)esc_1 = 2000;                                           //Limit the esc-1 pulse to 2000us.
    if(esc_2 > 2000)esc_2 = 2000;                                           //Limit the esc-2 pulse to 2000us.
    if(esc_3 > 2000)esc_3 = 2000;                                           //Limit the esc-3 pulse to 2000us.
    if(esc_4 > 2000)esc_4 = 2000;                                           //Limit the esc-4 pulse to 2000us.  
  }
  
  else{
    esc_1 = 1000;                                                           //If start is not 2 keep a 1000us pulse for esc-1.
    esc_2 = 1000;                                                           //If start is not 2 keep a 1000us pulse for esc-2.
    esc_3 = 1000;                                                           //If start is not 2 keep a 1000us pulse for esc-3.
    esc_4 = 1000;                                                           //If start is not 2 keep a 1000us pulse for esc-4.
  }
  
  //All the information for controlling the motor's is available.
  //The refresh rate is 250Hz. That means the esc's need there pulse every 4ms.
  while(micros() - loop_timer < 4000);                                      //We wait until 4000us are passed.
  loop_timer = micros();                                                    //Set the timer for the next loop.
  //PORTD |= B11110000;//4,5,6,7 ESCS HIGH
  PORTH |= B00011000;//set pin 6, 7 high(//Set digital poort 4, 5, 6 and 7 high.)
  PORTE |= B00001000;//Pin5
  PORTG |= B00100000;//Pin4
  timer_channel_1 = esc_1 + loop_timer;                                     //Calculate the time of the faling edge of the esc-1 pulse.
  timer_channel_2 = esc_2 + loop_timer;                                     //Calculate the time of the faling edge of the esc-2 pulse.
  timer_channel_3 = esc_3 + loop_timer;                                     //Calculate the time of the faling edge of the esc-3 pulse.
  timer_channel_4 = esc_4 + loop_timer;                                     //Calculate the time of the faling edge of the esc-4 pulse.
  
  while((PORTH & B00001000) || (PORTE & B00001000) || (PORTG & B00100000) || (PORTH & B00010000)) {
    //Stay in this loop until output 4,5,6 and 7 are low.
    esc_loop_timer = micros();       //Read the current time.
    //PORTD for UNO =4,5,6,7 ESCS basically, set them LOW
    if(timer_channel_1 <= esc_loop_timer)PORTG &= B11011111; //When the delay time is expired, digital port 4 is set low.
    if(timer_channel_2 <= esc_loop_timer)PORTE &= B11110111; //When the delay time is expired, digital port 5 is set low.
    if(timer_channel_3 <= esc_loop_timer)PORTH &= B11110111; //When the delay time is expired, digital port 6 is set low.
    if(timer_channel_4 <= esc_loop_timer)PORTH &= B11101111; //When the delay time is expired, digital port 7 is set low.
  }
  Serial.print("Roll");
  Serial.println(angle_roll_output);
  Serial.print("Pitch");
  Serial.println(angle_pitch_output);
  Serial.print("Yaw");
  Serial.println(gyro_yaw_input);
}
ISR(PCINT0_vect){
   current_timer=micros();
  //Channel 1=========================================
  if(last_channel_1 == 0 && PINB & B00010000){   //PCINT4   //Input 10 changed from 0 to 1.
    last_channel_1 = 1;                                                   //Remember current input state.
    timer_1 = current_timer;                                               //Set timer_1 to current_time.
  }
  else if(last_channel_1 == 1 && !(PINB & B00010000)){                                             //Input 8 is not high and changed from 1 to 0.
    last_channel_1 = 0;                                                     //Remember current input state.
    receiver_input_channel_1 = current_timer - timer_1;                             //Channel 1 is current_time - timer_1.
  }
  //Channel 2=========================================
  if(last_channel_2 == 0 && PINB & B00100000){//PCINT5    //Input 11 changed from 0 to 1.
    last_channel_2 = 1;                                                   //Remember current input state.
    timer_2 = current_timer;                                               //Set timer_2 to current_time.
  }
  else if(last_channel_2 == 1 && !(PINB & B00100000)){                                           //Input 9 is not high and changed from 1 to 0.
    last_channel_2 = 0;                                                     //Remember current input state.
    receiver_input_channel_2 = current_timer - timer_2;                             //Channel 2 is current_time - timer_2.
  }
  //Channel 3=========================================
    if(last_channel_3 == 0 && PINB & B01000000){ //PCINT6  //Input 12 changed from 0 to 1.
                             //Input 10 changed from 0 to 1.
      last_channel_3 = 1;                                                   //Remember current input state.
      timer_3 = current_timer;                                             //Set timer_3 to current_time.
    }
  else if(last_channel_3 == 1 && !(PINB & B01000000)){                                        //Input 10 is not high and changed from 1 to 0.
    last_channel_3 = 0;                                                     //Remember current input state.
    receiver_input_channel_3 = current_timer - timer_3;                             //Channel 3 is current_time - timer_3.

  }
  //Channel 4=========================================
    if(last_channel_4 == 0 && PINB & B10000000){//PCINT7 //Input 13 changed from 0 to 1.
                                       //Input 11 changed from 0 to 1.
      last_channel_4 = 1;                                                   //Remember current input state.
      timer_4 = current_timer;//Set timer_4 to current_time.
    }
  else if(last_channel_4 == 1 && !(PINB & B10000000)){                                                     //Input 11 is not high and changed from 1 to 0.
    last_channel_4 = 0;                                                     //Remember current input state.
    receiver_input_channel_4 = current_timer - timer_4;                             //Channel 4 is current_time - timer_4.
  }
}
void readGyro(){
   // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-8g, we need to divide the raw values by 4096, according to the datasheet
  acc_x = (Wire.read() << 8 | Wire.read()) / 4096.0; // X-axis value
  acc_y = (Wire.read() << 8 | Wire.read()) / 4096.0; // Y-axis value
  acc_z = (Wire.read() << 8 | Wire.read()) / 4096.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
 // accAngleX = (atan(acc_y / sqrt(pow(acc_x, 2) + pow(acc_z, 2))) * 180 / PI) +AccErrorX; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  //accAngleY = (atan(-1 * acc_x / sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * 180 / PI) + AccErrorY; // AccErrorY ~(-1.58)*/
 //subtract error?
 
 
  /*// === Read gyroscope data === //
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = GyroX; //* elapsedTime; // deg/s * s = deg
  //premature increment on gyroangleX not in accelerometerangleX
  gyroAngleY = GyroY;// * elapsedTime;
  yaw = GyroZ; //* elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX;
  pitch = 0.96 * gyroAngleY; */
  previousTime = current_time;        // Previous time is stored before the actual time read
  current_time = millis();            // Current time actual time read
  elapsedTime = (current_time - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false); 
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 65.5; // For a 500deg/s range we have to divide first the raw value by 65.5, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 65.5;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 65.5;
  // Correct the outputs with the calculated error values
  GyroX = (GyroX -GyroErrorX)*1; // GyroErrorX ~(-0.56)
  GyroY = (GyroY -GyroErrorY)*-1; // GyroErrorY ~(2)
  GyroZ = (GyroZ -GyroErrorZ)*-1; // GyroErrorZ ~ (-0.8)
  //Result already in degs/s
}
void calculate_pid(){
  //Roll calculations
  pid_error_temp = angle_roll_output - pid_roll_setpoint;
  pid_i_mem_roll += pid_i_gain_roll * pid_error_temp;
  if(pid_i_mem_roll > pid_max_roll)pid_i_mem_roll = pid_max_roll;
  else if(pid_i_mem_roll < pid_max_roll * -1)pid_i_mem_roll = pid_max_roll * -1;
  
  pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll + pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
  if(pid_output_roll > pid_max_roll)pid_output_roll = pid_max_roll;
  else if(pid_output_roll < pid_max_roll * -1)pid_output_roll = pid_max_roll * -1;
  
  pid_last_roll_d_error = pid_error_temp;
  
  //Pitch calculations
  pid_error_temp = angle_pitch_output - pid_pitch_setpoint;
  pid_i_mem_pitch += pid_i_gain_pitch * pid_error_temp;
  if(pid_i_mem_pitch > pid_max_pitch)pid_i_mem_pitch = pid_max_pitch;
  else if(pid_i_mem_pitch < pid_max_pitch * -1)pid_i_mem_pitch = pid_max_pitch * -1;
  
  pid_output_pitch = pid_p_gain_pitch * pid_error_temp + pid_i_mem_pitch + pid_d_gain_pitch * (pid_error_temp - pid_last_pitch_d_error);
  if(pid_output_pitch > pid_max_pitch)pid_output_pitch = pid_max_pitch;
  else if(pid_output_pitch < pid_max_pitch * -1)pid_output_pitch = pid_max_pitch * -1;
    
  pid_last_pitch_d_error = pid_error_temp;
    
  //Yaw calculations
  pid_error_temp = gyro_yaw_input - pid_yaw_setpoint;
  pid_i_mem_yaw += pid_i_gain_yaw * pid_error_temp;
  if(pid_i_mem_yaw > pid_max_yaw)pid_i_mem_yaw = pid_max_yaw;
  else if(pid_i_mem_yaw < pid_max_yaw * -1)pid_i_mem_yaw = pid_max_yaw * -1;
  
  pid_output_yaw = pid_p_gain_yaw * pid_error_temp + pid_i_mem_yaw + pid_d_gain_yaw * (pid_error_temp - pid_last_yaw_d_error);
  if(pid_output_yaw > pid_max_yaw)pid_output_yaw = pid_max_yaw;
  else if(pid_output_yaw < pid_max_yaw * -1)pid_output_yaw = pid_max_yaw * -1;
    
  pid_last_yaw_d_error = pid_error_temp;
}
void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  c=0;
  while (c < 2000) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    acc_x = (Wire.read() << 8 | Wire.read()) / 4096.0; // X-axis value
    acc_y = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    acc_z = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    // Sum all readings
     AccErrorX = AccErrorX + ((atan((acc_y) / sqrt(pow((acc_x), 2) + pow((acc_z), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (acc_x) / sqrt(pow((acc_y), 2) + pow((acc_z), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 2000;
  AccErrorY = AccErrorY / 2000;
  c = 0;
  // Read gyro values 200 times
  while (c < 2000) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();//read values from the first byte, shift them left by 8 bits, then load the rest of the bytes
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 65.5);
    GyroErrorY = GyroErrorY + (GyroY / 65.5);
    GyroErrorZ = GyroErrorZ + (GyroZ / 65.5);//500d/s
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 2000;
  GyroErrorY = GyroErrorY / 2000;
  GyroErrorZ = GyroErrorZ / 2000;
  // Print the error values on the Serial Monitor
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);

  Serial.println(AccErrorX);
  Serial.println(AccErrorY);
  Serial.println(AccErrorZ);

}
void printSignal(){
  Serial.print("Roll:");
  if (receiver_input_channel_1-1480 <0)Serial.print("<<<");
  else if(receiver_input_channel_1-1520>0)Serial.print(">>>");
  else Serial.print("___+___");
  Serial.print(receiver_input_channel_1);

  Serial.print("Nick");
  if (receiver_input_channel_2-1480 <0)Serial.print("vvv");
  else if(receiver_input_channel_2-1520>0)Serial.print("^^^");
  else Serial.print("___+___");
  Serial.print(receiver_input_channel_2);

  Serial.print("Gas");
  if (receiver_input_channel_3-1480 <0)Serial.print("vvv");
  else if(receiver_input_channel_3-1520>0)Serial.print("^^^");
  else Serial.print("___+___");
  Serial.print(receiver_input_channel_3);

  Serial.print("Yaw");
  if (receiver_input_channel_4-1480 <0)Serial.print("<<<");
  else if(receiver_input_channel_4-1520>0)Serial.print(">>>");
  else Serial.print("___+___");
  Serial.println(receiver_input_channel_4);
}
void printOutput(){
  // Print the values on the serial monitor
  Serial.print("   pitch");
  if (GyroY>=0)Serial.print("+");
  Serial.print(GyroY);
  if(GyroY-2>0)Serial.print("NoU");
  else if(GyroY+2<0)Serial.print("NoD");
  else Serial.print("-----------");
  Serial.print("   roll");
  if (GyroX>=0)Serial.print("+");
  Serial.print(GyroX);
  if(GyroX-2>0)Serial.print("RwD");
  else if(GyroX+2<0)Serial.print("RwU");
  else Serial.print("-----------");
  Serial.print("   Yaw");
  if (GyroZ>=0)Serial.print("+");
  Serial.print(GyroZ);
  if(GyroZ-2>0)Serial.println("NoR");
  else if(GyroZ+2<0)Serial.println("NoL");
  else Serial.println("-----------");
}
void setupMPUregisters(){
  //Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset to the mpu  - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission

  //Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                   //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  //Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  //Set the register bits as 00010000 (1000deg/s full scale)    //Wire.write(0x10);                   
  //Configure the gyro (500dps full scale)
  Wire.write(0x08);               //Set the requested starting register
  Wire.endTransmission(true);

}
