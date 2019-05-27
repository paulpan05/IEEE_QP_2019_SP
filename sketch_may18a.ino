#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 53
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define echoPin 7    // Echo Pin
#define trigPin 8    // Trigger Pin

// Enum for states
#define CARD 0
#define DOOR 1

// Servo variables and function
int servopin = 6;      // Servo Pin
int myangle;         // initialize angle variable
int pulsewidth;      // initialize width variable
int val;
void servopulse(int servopin,int myangle) {  // define a servo pulse function
  pulsewidth=(myangle*11)+500;               // convert angle to 500-2480 pulse width
  digitalWrite(servopin,HIGH);               // set the level of servo pin as “high”
  delayMicroseconds(pulsewidth);             // delay microsecond of pulse width
  digitalWrite(servopin,LOW);                // set the level of servo pin as “low”
  delay(20-pulsewidth/1000);
}

// Distance sensor variables
int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration;
double distance; // Duration used to calculate distance

// Current state
unsigned int current = CARD;

void setup() {
  pinMode(servopin,OUTPUT);      // set servo pin as “output”
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);            // Initiate a serial communication
  SPI.begin();                   // Initiate  SPI bus
  mfrc522.PCD_Init();            // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  Serial.println("servo=o_seral_simple ready" ) ;
}

void loop() {
  if ( current == CARD ) {
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) return;
  
    //Show UID on serial monitor
    Serial.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    if (content.substring(1) == "04 48 4F 52 08 5B 80" ||
      content.substring(1) == "04 40 5D 52 08 5B 80" ) {
      Serial.println("Authorized access");
      Serial.println();
      val = 8 * (180 / 9);
      for ( int i = 0; i <=50; ++i ) {
        servopulse(servopin,val);
      }
      current = DOOR;
    } else {
      Serial.println(" Access denied");
      Serial.println();
    }
  } else if ( current == DOOR ) {
    Serial.println("Case: DOOR");
    Serial.println();
    /* The following trigPin/echoPin cycle is used to determine the
       distance of the nearest object by bouncing soundwaves off of it. */ 
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
 
    // Calculate the distance (in cm) based on the speed of sound.
    distance = duration/58.2;
    val = 2 * (180 / 9);
    if ( distance < 0.5 && distance <= minimumRange ) {
      for ( int i = 0; i <=50; ++i ) {
        servopulse(servopin,val);
      }
      current = CARD;
    }
  }
}
