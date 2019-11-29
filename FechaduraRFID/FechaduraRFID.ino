#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
#include <Servo.h>
/*
   Programa que faz uma fechadura. A chave usada para fechar deve ser a mesma para abrir

   LED fica acesso quando porta aberta e pisca se a chave for errada
      Para led foi utilizado analogwrite pois eu estava sem resistor
   PINOS DO RFID
      PIN 10 - SDA
      PIN 13 - Sck
      PIN 11 - Mosi
      PIN 12 - Miso
      PIN  9 - RST
*/
#define SS_PIN 10 //PINO SDA
#define RST_PIN 9 //PINO DE RESET
#define LED 5
#define SERVO 6

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS
Servo fechadura;  // create a servo object

int aberto;
String cartao_lido;
String senha;

// Função que fechar o armario
void fechar() {
  aberto = 0;
  analogWrite(LED, LOW);
  fechadura.write(200);
}
// Função que abre o armario
void abrir() {
 aberto = 1;
 analogWrite(LED, 120);
 fechadura.write(0);
}
// Função que lê o cartão e retorna a string com a representação dela
String lerCartao() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return ""; //RETORNA PARA LER NOVAMENTE

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
  return strID;
}

void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  fechadura.attach(SERVO);
  pinMode(LED, OUTPUT);
  analogWrite(LED, 120);
  aberto = 1;
  abrir();
}

void loop() {

  cartao_lido = lerCartao();
  if (cartao_lido != "") {
    if (aberto) {
      senha = cartao_lido;
      fechar();
    } else {
      if (cartao_lido == senha) {
        abrir();
      } else {
        for (int i = 0 ; i < 3; i++) {
          analogWrite(LED, 120);
          delay(100);
          analogWrite(LED, LOW);
          delay(100);
        }
      }
    }
    Serial.print("Identificador (UID) da tag: "); //IMPRIME O TEXTO NA SERIAL
    Serial.println(cartao_lido); //IMPRIME NA SERIAL O UID DA TAG RFID
  }
}
