/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//byte rowPins[ROWS] = {9, 8, 7, 6}; 
//byte colPins[COLS] = {5, 4, 3, 2}; 
byte rowPins[ROWS] = {8, 7, 6, 5}; 
byte colPins[COLS] = {4, 3, 2, 1}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 



#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// ##########

class Player {
  private:
    long unsigned cardID;
    unsigned balance;
    bool isActive;
    
  public:
    Player(long unsigned cardID){
      Serial.println(F("Starting Player"));
      this->cardID = cardID;
      init();
      Serial.println(F("Ending Player"));
    }
    void init(){
      Serial.println(F("starting init for Player"));
      balance = 500;
      if (cardID > 0){
        isActive = true;
        Serial.print(F("Initialized Player with card="));
        Serial.print(cardID);
        Serial.print(F(" and balance="));
        Serial.println(balance);
      }else{
        isActive = false;
        Serial.println(F("Skipped Player"));
      }
      
      Serial.println(F("ending init for Player"));
    }
//    void addToBalance(unsigned ammount){
//      
//    }
    long unsigned getCardID(){
      return cardID;
    }
    
};

long unsigned getRFIDCardID(){ // todo: later add the port int RFIDPort, since we are going toh ve 2 RFID readers
  Serial.println(F("-- Started getRFIDCardID"));
  bool abort = false;
  while (! mfrc522.PICC_IsNewCardPresent()){
    char customKey = customKeypad.getKey();
  
    if (customKey) {
       Serial.println(customKey);
      if (customKey == '#') {
 
        abort = true; 
        Serial.println("aborting");
        break;
      }


    }
  }
  Serial.println(F("-- -- PICC_IsNewCardPresent "));

  if (abort){
    return 0;
  }
  while (! mfrc522.PICC_ReadCardSerial()){
    delay(500);
  }
  Serial.println(F("-- -- PICC_ReadCardSerial "));
  
  mfrc522.PICC_HaltA();
  Serial.print("UID tag :");
  String content= "";
  // mfrc522.uid.size
  int max_size = 3;
  if (mfrc522.uid.size < 3) {
    max_size = mfrc522.uid.size;
  }
  for (byte i = 0; i < max_size; i++) 
  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], DEC);
//     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], DEC));
  }
//  Serial.println();
  unsigned long cardUid = content.toInt();

  return cardUid;
  
  
}


void gameSetup(){
  Serial.println(F("Initializing game setup"));
  Player player1(getRFIDCardID());
  Player player2(getRFIDCardID());
  Player player3(getRFIDCardID());
  Player player4(getRFIDCardID());
  
  
  Serial.println(F("game setup complete"));  
}

// ##########
void setup() {
	Serial.begin(115200);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
//	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
//	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
 gameSetup();
}





void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
//	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  mfrc522.PICC_HaltA();
  



  
  Serial.print("UID tag :");
  String content= "";
  // mfrc522.uid.size
  int max_size = 3;
  if (mfrc522.uid.size < 3) {
    max_size = mfrc522.uid.size;
  }
  for (byte i = 0; i < max_size; i++) 
  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], DEC);
//     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], DEC));
  }
//  Serial.println();
  unsigned long cardUid = content.toInt();
  Serial.println(cardUid);
//  Serial.println();
  int playerNr;
  Serial.print("Player : ");
  switch (cardUid){
    case 51219132:  // circle TAG 
//      Serial.print(" --- 1 ---");
      playerNr = 1;
      break;
    case 13115268:  // circle TAG 
//      Serial.print(" --- 2 ---");
      playerNr = 2;
      break;
     case 513294:   // olmo visa comdirect debit
//      Serial.print(" --- 3 ---");
      playerNr = 3;
      break;
    case 22548174:   // olmo Wise green card 1520
//      Serial.print(" --- 4 ---");
      playerNr = 4;
      break;

     case 12792202:   // lino Visa comdirect 3105
//      Serial.print(" --- 5 ---");
      playerNr = 5;
      break;

//     case 12792202:   // olmo Wise green card 1520
////      Serial.print(" --- 5 ---");
//      playerNr = 5;
//      break;
      
     default:
//      Serial.print(" --- 0 ---");
      playerNr = 0;
      break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
  }
  Serial.println(playerNr);
  Serial.println();
}
