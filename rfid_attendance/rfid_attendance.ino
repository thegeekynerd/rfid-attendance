
#include<SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9             // Configurable
#define SS_PIN          10            // Configurable
#define DEBUG true

SoftwareSerial wifi(2,3); //rx, tx

byte readCard[4];                     // For storing the introduced card details
MFRC522 mfrc522(SS_PIN, RST_PIN);     // Create MFRC522 instance

String rfid="";


void setup() 
{
  
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  Serial.println("Wifi Started");
  wifi.begin(9600);

  sendData("AT+RST\r\n",3000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG);
  
}

void loop()
{
  rfid1();
  
  
}


void rfid1() 
{ 
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
 getInfo();
 for (int i = 0; i < 4; i++) 
  { 
    rfid+=(String)readCard[i];
  }
    Serial.println(rfid);
    iot();
    rfid="";
    
    delay(5000);
    
  }
void getInfo()
{
  for (int i = 0; i < 4; i++) 
  { 
    readCard[i] = mfrc522.uid.uidByte[i];
    
  }

}
void iot() 
{
      sendData("AT+CIPMUX=1\r\n",150,DEBUG); //set multiplexer to multiple connections
      
      sendData("AT+CIPSTART=1,\"TCP\",\"192.168.0.5\",80\r\n",100,DEBUG); // set the host website while mentioning the host and port number and the connection number
      
      String cipstart="GET /web/increment.php?rfid=";
      cipstart+=rfid;
      cipstart+=" HTTP/1.1\r\n"; //GET request in a String containing the information of the RFID card tapped
      
      String HOST="HOST: 192.168.0.5\r\n"; //mentioning the host address in the massage to be sent
      
      String conn="CONNECTION: keep-alive\r\n\r\n\r\n";
      int l=cipstart.length()+HOST.length()+conn.length(); //finding the total length of the massage to be sent
      
      String cipsend="AT+CIPSEND=1,";
      cipsend+=(String)l;
      cipsend+="\r\n"; //Sending information about bytes to be sent over the connection
      
      sendData(cipsend,250,DEBUG);
      
      sendData(cipstart,100,DEBUG);
      
      sendData(HOST,100,DEBUG);
      
      sendData(conn,400,DEBUG);//self-explanatory, sending the above string to the esp through the function sendData(command,timeout,DEBUG);
      
      sendData("AT+CIPCLOSE=1\r\n",300,DEBUG); //closing the connection number 1;
  
}

String sendData(String command, const int timeout, boolean debug)
{
  String response="";
  wifi.print(command);
  delay(timeout);
  while(wifi.available())
  {
    char c=wifi.read();
    response+=c;
  }
  if(debug)
  {
    Serial.println(response);
  }
  
  return response;
}

