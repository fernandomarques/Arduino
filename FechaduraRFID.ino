#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
#include <Servo.h>

#define SS_PIN 10 //PINO SDA
#define RST_PIN 9 //PINO DE RESET
#define LED 5 
#define SERVO 6
MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS
Servo fechadura;  // create a servo object

int aberto; 
void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  fechadura.attach(SERVO);
  pinMode(LED, OUTPUT);
  aberto = 1;
}
void fechar(){
 for(int angle = 180; angle >= 0; angle--){
  fechadura.write(angle);  
  delay(15);
 }
 
}
void abrir(){
for(int angle = 0; angle <= 180; angle++){ 
 fechadura.write(angle);
   delay(15);
}
}

String lerCartao(){
   if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return ""; //RETORNA PARA LER NOVAMENTE
 
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
  return strID;
}
void loop() {
  String strId;
  String senha = "DE:25:EC:73";
  strId = lerCartao();
 
  if(aberto){
    analogWrite(LED,120);
  }else{
    analogWrite(LED,LOW);
  }
  
  if(strId != ""){
    if(aberto){
      Serial.print("entrou");
      //senha = strId;
      analogWrite(LED,LOW);
      aberto = 0;
      fechar();
    }else{
      if(strId == senha){
        aberto = 1;
        analogWrite(LED,120);
        abrir();
      }else{
        Serial.println("Senha invalida");
        for(int i = 0 ; i < 3; i++){
          analogWrite(LED,120);
          delay(100);
          analogWrite(LED,LOW);
          delay(100);          
        }

      }
    }
    Serial.print("Identificador (UID) da tag: "); //IMPRIME O TEXTO NA SERIAL
    Serial.println(strId); //IMPRIME NA SERIAL O UID DA TAG RFID
  }
 
  
}
