#include <SoftwareSerial.h>  
SoftwareSerial miBT(10, 11);  // pin 10 como RX, pin 11 como TX

void setup(){
  Serial.begin(9600);   // comunicacion de monitor serial a 9600 bps
  miBT.begin(9600);    // comunicacion serie entre Arduino y el modulo a 38400 bps
  Serial.println("Listo");  // escribe Listo en el monitor 
  
}

void loop(){

  if (miBT.available())       // si hay informacion disponible desde modulo
     Serial.write(miBT.read());   // lee Bluetooth y envia a monitor serial de Arduino

  if (Serial.available()) // si hay informacion disponible desde el monitor serial
    miBT.write(Serial.read());   // lee monitor serial y envia a Bluetooth

}
/*
AT+NAME? <Nombre>
AT+PSWD? <"Pin">
AT+UART? <Baud> ,< StopBit>,< Parity>
AT+ROLE? Maestro o Esclavo
AT+CMODE? Configurar el modo de conexión
AT+BIND? <Address>
AT+VERSION?
AT+ADDR? <dirección> 
AT+RESET <SAVE>
*/
