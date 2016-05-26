#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define pinR 6
#define pinA 5
#define pinV 3
#define pinBuzzer 7

String COLOR_COMMAND = "COLOR";
String SPEED_COMMAND = "SPEED";
String SOUND_COMMAND = "SOUND";

String buff;
 
MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[3];

void setup() 
{
  pinMode(pinR,OUTPUT);
  pinMode(pinA,OUTPUT);
  pinMode(pinV,OUTPUT);
  pinMode(pinBuzzer,OUTPUT);
  setColor(0,0,0);
  
  Serial.begin(9600);
  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();      // Init MFRC522 

  for (byte i = 0; i < 6; i++) 
    {  key.keyByte[i] = 0xFF;
    }

}
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], DEC);
  }
}
//------------------------------------------------------------------

void loop() {

  checkSerial();

  // Buscamos tarjetas
  if (rfid.PICC_IsNewCardPresent()) {

  // SI la encuentra la leemos
  if (rfid.PICC_ReadCardSerial()) {

  Serial.print(F("type:"));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print(rfid.PICC_GetTypeName(piccType));
  Serial.print(F(";"));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
      {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
      }

      Serial.print("dec:");
      printDec(rfid.uid.uidByte, rfid.uid.size);
      Serial.print(";");
      Serial.print("hex:");
      printHex(rfid.uid.uidByte, rfid.uid.size);
      Serial.print(";");
      Serial.println();
      

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) 
    {
     
       rfid.PICC_HaltA();
       rfid.PCD_StopCrypto1();
    }

  }

  }
}

void setColor(int red, int green, int blue) {

  analogWrite(pinR,255 - red);
  analogWrite(pinV,255 - green);
  analogWrite(pinA,255 - blue);

  
}

void checkSerial() {
    char input = Serial.read();
    //Si se detecta salto de linea, interpreta el comando
    if(input == '\n') {

      char str[20];
      char * pch;
      
      buff.toCharArray(str,20);
      
      pch = strtok(str,";");

      String compare = pch;
      
      if(compare == COLOR_COMMAND){
        
        char * red = strtok (NULL, ";");
        
        char * green = strtok (NULL, ";");

        char * blue = strtok (NULL, ";");

        char * durationMillis = strtok (NULL, ";");

        setColor(atoi(red),atoi(green),atoi(blue));
        delay(atoi(durationMillis));
        setColor(0,0,0);
        
        
      }else if(compare == SPEED_COMMAND) {

        pch = strtok (NULL, ";");
        Serial.println(pch);
        
      }else if(compare == SOUND_COMMAND) {

        char * durationMillis = strtok(NULL, ";");
        
        digitalWrite(pinBuzzer,HIGH);
        delay(atoi(durationMillis));
        digitalWrite(pinBuzzer,LOW);

      }

      buff = "";
    }
    else {
      //Si es distinto de -1 lee
      if(input != -1) {
         buff += input;
      }
    }
}

