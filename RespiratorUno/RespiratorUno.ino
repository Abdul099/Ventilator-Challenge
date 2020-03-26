/*
 * Ventilator Challenge: UNO Script
 * Author: Abdullatif Hassan
 * Date Started: March 25, 2020
 * 
 * Descrpition: This is the first crude version for the controls that the Arduino UNO board uses in the Ventilator System. The aim behind this version is to outline the control system that
 *              will be used to control the operation of the vebtilator. Due to the lack of sensor data at the time being, mock data is used throughout the implementation. 
 * 
 * 
 */

bool start;//1 for started 0 for OFF
int bpm;
float IP;//might use ints for pressure instead
float targetIP;
bool expValve;//1 for open 0 for closed
long targetTime;
long lastBreathStart;
long criticalIP;
long EP;
long neededEP;
int motorPosition;//mock variable for motor control 0--> resting position

void calibrate(){ // calibration function --> will work on later
}
void setup() {
  bpm = 20;
  targetIP = 40;
  expValve = 1;
  lastBreathStart = -(60 / bpm) * 1000;
  motorPosition = 0;
  calibrate();
}

void loop() {
  //step 1
  if (start) { 
    //step 2: need to read inputs from sensors, now using mock data 
    bpm = 23;
    IP = 3;
    EP = 5;

    //step 3: the actual cycle
    //3a

    if (millis() - lastBreathStart > (60 / bpm) * 1000 || IP<-5) { //if enough time has passed from last breath or if patient attempts to breath inspiratory pressure below -5 cmH20
      expValve = 0; //3b close
     
      while (IP < targetIP) {//3c: deliver breath as long as IP is still low
        
         if (IP >= criticalIP){
          Serial.println("Warning High Inspiratory Pressure"); //this will be sent to the lcd screen in later versions
          break;
        }
        else{
           motorPosition++;
        }

        }
        motorPosition = 0; //reset motor to resting position
        expValve = 1; //open expiratory valve
        delay(1000);//will adjust the value later
        if(EP<neededEP) Serial.println("Warning Low  Pressure");
        
        IP = IP +1; //mock IP reading
        bpm = 18; //mock data
        EP = 20;//mock data
        
        Serial.println("Printing to lcd"); // communicate with other arduino --> will work on it later 
    }
    //end of step 3 (cycle)--> go back to step 1




  }

}
