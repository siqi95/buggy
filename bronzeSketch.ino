// hardware interface
#define CTRL_PIN 3
#define IR_PIN 2

//For buggyControl() to make the buggy do different actions
int RUN = 4;
int STOP = 2;
int LEFT = 6;
int RIGHT = 8;

String inputString = "";
boolean stringComplete = false;
volatile boolean irInterrupt = false;    //IR sensor on buggy
volatile byte state = HIGH;  


int gantryCounter = 0;        //for counting how many gantries, to know when to call the parking function

const int pingPin = A4;      // the pin number for the signal
const int powerPin = A3;    // the pin number for the power
const int gndPin = A2;       // the pin number for the gnd
const int enablePin  =  3;
unsigned long currentMillis;
// Variables will change :
float pulse;
boolean ultraSonics = true;   //flag to turn on/off the ultrasonics

unsigned long previousMillis = 0;        // will store last time ultrasonics checked was updated

const long interval = 1000;           // interval at which to check the ultrasonics (milliseconds)

 float distance=pulse/58;

void setup() {
  // put your setup code here, to run once:
  // set the pins INPUT or OUTPUT
  pinMode(CTRL_PIN, OUTPUT);
  digitalWrite(CTRL_PIN, LOW);
  pinMode(IR_PIN, INPUT);  attachInterrupt(digitalPinToInterrupt(IR_PIN), blink9, RISING);    

  //set up serial comms
  Serial.begin(9600); //Initiate Serial communication at 9600 baud
  Serial.print("+++"); // Enter xbee AT command mode
  delay(1500); // Guard time
  Serial.println("ATID 3511, CH C, CN"); // Our specific PAN ID is 3511. Channel: C
  delay(1100);
 
  while (Serial.read() != -1) {}; //get rid of OKs

  // set the digital pin as output:
  pinMode(pingPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  pinMode(gndPin, OUTPUT);
  //Power the module
  digitalWrite(powerPin, HIGH);
  digitalWrite(gndPin, LOW);
  digitalWrite(enablePin, HIGH);

  Serial.begin(9600);

}


void loop() {


  digitalWrite(CTRL_PIN, HIGH);     

      if (stringComplete) {                       //if what is written into the pc console is any version of "Go"
        inputString.toLowerCase();
          if (inputString.indexOf("go") != -1) {
            writeXbee("High");                    //Message back to console
            buggyControl(RUN);

      }
          else if (inputString.indexOf("stop") != -1) {
            writeXbee("Low");
            buggyControl(STOP);

      }

          else if (inputString.indexOf("park") != -1) {
            writeXbee("Buggy is parking.");
            buggyPark();

      }
          else {
            writeXbee("NOT MESSAGE\n");
      }


     stringComplete = false;
     inputString = "";

   }
 
 

  if (irInterrupt) {         //If the buggy comes under a Gantry (I.E IR sensor on buggy detects IR light from above)                 
    
    buggyControl(STOP);
    int gantry = readGantry();

    writeXbee("G" + String(gantry));
    delay(1000);

         if (gantryCounter == 2) {     //Buggy comes under the second Gantry twice and must park  
      
            writeXbee("Two laps completed.\n");
            buggyPark();
            digitalWrite(pingPin,LOW);

            irInterrupt = false;
    }


    else {
      
      irInterrupt = false;

      writeXbee("High2");        
      buggyControl(RUN);
    }

  }

  
  // Ultrasonics
  currentMillis = millis();

 if (currentMillis - previousMillis >= interval) {
    // save the last time you checked the ultrasonics
    previousMillis = currentMillis;

    if (ultraSonics) //ultrasonics turned on (true/false)
    {


      if (ultraSonicDet()) { //if object detected


        Serial.println("object detected");
        writeXbee("Object Detected");
        buggyControl(STOP);
        
        while (ultraSonicDet() ){
          writeXbee("Object Detected");
          
        }

        
           buggyControl(RUN);
      }
       
    }

  }
}

//End of the main loop

void serialEvent() {                //Messages from the PC Xbee
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

// write to Xbee

void writeXbee(String message) {
  Serial.println(message);    //This prints things onto the PC
  Serial.flush();
}

void buggyControl(int x) {     //buggyControl() works by sending pulses to the BCC and then the HBridge

  digitalWrite(CTRL_PIN, LOW);
  delay(2);
  digitalWrite(CTRL_PIN, HIGH);
  delay(x);                         //The Hbridge makes the buggy do different functions
  digitalWrite(CTRL_PIN, LOW);      //depending on how long the pulses are (Thats the "x")
  delay(2);
}

void blink9(){
irInterrupt = true;
}

int readGantry () {                          
  while (digitalRead(IR_PIN) != LOW);
  int duration = pulseIn(2, HIGH);

  if ( duration >= 500 && duration <= 1500) {

    return 1;
  }

  else if ( duration >= 1500 && duration <= 2500) {
   gantryCounter++;                                    //Buggy must park on the second lap after gantry 2   
    return 2;
  }

  else if ( duration >= 2500 && duration <= 3500) {
    
    return 3;
  }



}

void buggyPark() {      //buggyPark() calls on the buggyControl function to generate actions such as left override for different amounts of time
  buggyControl(LEFT);
  delay(3000);
  buggyControl(RUN);    //Buggy goes along inside track for a little while
  delay(4000);
  buggyControl(STOP);   //Then stops at a parking space.

}


boolean ultraSonicDet()
{

        pinMode(pingPin, OUTPUT);

        digitalWrite(pingPin, HIGH);
        delayMicroseconds(10);

        digitalWrite(pingPin,LOW);       //Turn ultrasonic pulse generator off

        delayMicroseconds(10);


        pinMode(pingPin, INPUT);

        delayMicroseconds(10);
        pulse = pulseIn(pingPin, HIGH);

        float distance=pulse/58;
 
        if (distance < 10)               //10cm
             return true;
         else
             return false;


}


