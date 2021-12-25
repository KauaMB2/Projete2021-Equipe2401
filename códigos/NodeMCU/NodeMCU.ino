#include <SoftwareSerial.h>  
#include <Wire.h>     
#include "SSD1306Wire.h"     
#include <brzo_i2c.h>     
#include "SSD1306Brzo.h"     
SoftwareSerial dados(D6, D7);  
SSD1306Brzo display(0x3c, D3, D5);     
int Bpm = 0;     
int SpO2 = 0;     
int Estado = 0;     
int Temp = 0;     
int Ipm = 0; 
int oxigenacaoDiastole = 80; 
int oxigenacaoSistole = 120; 
#include "Ubidots.h"  
const char* UBIDOTS_TOKEN = "BBFF-zaYX8XsfFcZbwjhdzOVB5OBLi52OnS";  // Put here your Ubidots TOKEN  
const char* WIFI_SSID = "My ASUS_6562";      // Put here your Wi-Fi SSID  
const char* WIFI_PASS = "d184f6c3fa2f";      // Put here your Wi-Fi password  
Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);  
#define DEMO_DURATION 3000     
typedef void (*Demo)(void);     
int demoMode = 0;     
int counter = 1;     
void setup() {     
  Serial.begin(115200);    
  dados.begin(57600);   
  display.init();     
  display.flipScreenVertically();     
  display.setFont(ArialMT_Plain_10);   
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);    
}     
  String msg = ""; //Variavel da mensagem     
  String stringBpm = ""; //String do Bpm     
  String stringSpO2 = "";    //String do SpO2 
  String stringEstado = "";    //String do Estado 
  String stringTemp = "";   //String da Temp 
  String stringIpm = "";  //String da Ipm 
  int pos1, pos2, pos3, pos4, pos5;// Variáveis das posições     
void drawFontFaceDemo(){     
  display.setTextAlignment(TEXT_ALIGN_LEFT);     
  display.drawString(0, 0, "Bpm:: " + stringBpm);     
  display.drawString(0, 10, "SpO2: " + stringSpO2 + "%");    
  display.drawString(0, 20, "Estado: " + stringEstado);     
  display.drawString(0, 30, "Temperatura: " + stringTemp + "C");    
  display.drawString(0, 40, "Ipm: " + stringIpm);    
}     
Demo demos[] = {drawFontFaceDemo};     
int demoLength = (sizeof(demos) / sizeof(Demo));     
long timeSinceLastModeSwitch = 0;     
void loop() {     
  display.clear();     
  demos[demoMode]();     
  display.setTextAlignment(TEXT_ALIGN_RIGHT);     
  display.drawString(10, 128, String(millis()));     
  display.display();     
  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION){     
    demoMode = (demoMode + 1)  % demoLength;     
    timeSinceLastModeSwitch = millis();     
  }      
  if (dados.available() > 0) {     
    char c = dados.read();// Leitor do Serial Monitor     
    if (c != '#') {     
      msg = msg + c;     
    }     
    else  //fim da msg, c = #     //ex.: 123?67#     
    {     
      pos1 = msg.indexOf("?");// Define "pos1" como sendo "?"     
      stringBpm = msg.substring(0, pos1); // Define "dado" como sendo os valores entre 0 até "pos1"(?)       
      Bpm = stringBpm.toInt();// Transforma "dado" em variável com valor numérico     
         
      pos2 = msg.indexOf("*");// Define "pos1" como sendo "?"     
      stringSpO2 = msg.substring(pos1 + 1, pos2); // Define "dado" como sendo os valores entre 0 até "pos1"(?)  
      SpO2 = stringSpO2.toInt();// Transforma "dado" em variável com valor numérico     
         
      pos3 = msg.indexOf(";");// Define "pos2" como sendo "#"     
      stringEstado = msg.substring(pos2 + 1, pos3); // Define "dado" como sendo os valores entre "pos1"(?) até "pos2"(#)     
      Estado = stringEstado.toInt();// Transforma "dado" em variável com valor numérico    
      pos4 = msg.indexOf("~");// Define "pos2" como sendo "#"     
      stringTemp = msg.substring(pos3 + 1, pos4); // Define "dado" como sendo os valores entre "pos1"(?) até "pos2"(#)     
      Temp = stringTemp.toInt();// Transforma "dado" em variável com valor numérico   
      pos5 = msg.indexOf("#");// Define "pos2" como sendo "#"     
      stringIpm = msg.substring(pos4 + 1, pos5); // Define "dado" como sendo os valores entre "pos1"(?) até "pos2"(#)     
      Ipm = stringIpm.toInt();// Transforma "dado" em variável com valor numérico    
      msg = ""; // Transforma a variável de texto "msg" em uma variável vazia     
    } 
  } 
  ubidots.add("batimentos", Bpm); 
  ubidots.add("SpO2", SpO2); 
  ubidots.add("queda", Estado); 
  ubidots.add("ipm", Ipm); 
  ubidots.add("temperatura", Temp); 
  ubidots.add("pressao diastole", oxigenacaoDiastole); 
  ubidots.add("pressao sistole", oxigenacaoSistole); 
  bool bufferSent = false;  
  bufferSent = ubidots.send("vsmeter"); 
}
