#include <math.h>
#include <SoftwareSerial.h>

int RxD=10;
int TxD=11;

const int PinHumedad = A0;
const int PinTemperatura = A1;
SoftwareSerial Bluetooth(RxD,TxD );

//Informacion

String Serie       = "CE01";
String Name        = "Albaca";
int    Humedad     = 0;
float  celsius     = 0;                                       
String Serie2      = "";
int    contador    = 0;
String Entrada     = "";
const int Vcc      = 5;
String Datos[5]     = {"","","","",""};
String EnvioEstado = "";

void setup() {
  Bluetooth.begin(9600);
  Serial.begin(9600);
  pinMode(RxD, INPUT);  //Bluetooth
  pinMode(TxD, OUTPUT); //Bluetooth
  pinMode(13 , OUTPUT); //ampolleta
  pinMode(8  , OUTPUT);
  pinMode(PinHumedad    , INPUT);
  pinMode(PinTemperatura, INPUT);
  Serial.println("{'Activado':'" +Serie +"'}"); 
} 

void loop() {
  delay(1000);

  if(Bluetooth.available())    // Si llega un dato por el puerto BT se envía al monitor serial
  {
    Entrada = Bluetooth.readString();
  }
 
  if(Serial.available())  // Si llega un dato por el monitor serial se envía al puerto BT
  {
    Entrada = Serial.readString();
  }

  if(Entrada!="" )
  { 
    Analizar();
    Entrada ="";
  }
  
}

void Analizar(){
  AmpolletaOn();
  MedirHumedad();
  MedirTemperaturaSuelo();
  Addjson();
  AmpolletaOff(); 
}

void MedirHumedad(){
  int suma = 0;
  int porcentaje =0;
  int arrayHumedad[]= {0, 0, 0, 0, 0};
  for (int i = 0; i <= 4; i++) {
    int SensorValue = analogRead(PinHumedad); 
    if(Vcc==5)
    {
      porcentaje = map(SensorValue,75,1023,0,100); //5v
    }else if(Vcc==3)
    {
      porcentaje = map(SensorValue,50,700,0,100); //3v
    }
    arrayHumedad[i] = porcentaje;
    delay(200);
  }
  
  for (int i = 0; i <= 4; i++) {
    suma = suma+ arrayHumedad[i];
  }
  if(suma>0)
  {
     porcentaje = (suma/5);
  }
 
  porcentaje = 100-porcentaje;
  Humedad = porcentaje;

}

void MedirTemperaturaSuelo(){
  float arrayTemperatura[]= {0, 0, 0, 0, 0};
  int   Rc = 10000; //valor de la resistencia
  float A = 1.11492089e-3;
  float B = 2.372075385e-4;
  float C = 6.954079529e-8;
  float K = 2.5; //factor de disipacion en mW/C
  for (int i = 0; i <= 4; i++) {
    float raw = analogRead(PinTemperatura);
    float V =  raw / 1024 * Vcc;
    float R = (Rc * V ) / (Vcc - V);
    float logR  = log(R);
    float R_th = 1.0 / (A + B * logR + C * logR * logR * logR );
    float kelvin = R_th - V*V/(K * R)*1000;
    celsius = kelvin - 273.15;
    arrayTemperatura[i]=celsius;
    delay(200);
  }
  float suma=0;
  for (int i = 0; i <= 4; i++) {
    suma = suma + arrayTemperatura[i];
  }
  if(suma>0)
  {
    celsius = suma/5;  
  }
  
  if(Vcc==3)
  {
    celsius=celsius-(10);
  }

}

void AmpolletaOn(){
  digitalWrite(13,HIGH);
}
void AmpolletaOff(){
  digitalWrite(13,LOW);
}

void Addjson(){
  String info = "{'Serie':'" + Serie + "','Name':'" + Name +"','H':'" + Humedad +"','T':'" + celsius + "','C':'" + contador++ + "'}";
  getValue(Entrada);//realiza la array de datos entrante al array<Datos>  
  
  for (int i = 0; i < 5; i++)
  {
    //si el registro existe lo cambia
    if(Datos[i].indexOf(Serie)>0)
    {
      Datos[i]=info;i=100;
    }else if(Datos[i]=="")
    {
      Datos[i]=info;
      i=100;
    }
  }
  EnvioEstado ="[";  
  for (int i = 0; i < 5; i++)
  {
      if(Datos[i]!="")
      {
        EnvioEstado +=Datos[i]+";";
        Datos[i]="";
      }
  }
  EnvioEstado = EnvioEstado.substring(0,EnvioEstado.length()-1);
  EnvioEstado +="]";
  
  Bluetooth.println(EnvioEstado);
  Serial.println(EnvioEstado);

}

void getValue(String texto){
  texto.replace("[","");
  texto.replace("]","");
  texto.replace("\n","");
  texto.replace("\r","");
  while(texto.indexOf(';')>0)
  {
    int    pos   = texto.indexOf(';');
    String frace = texto.substring(0,pos);
    for (int i = 0; i < 5; i++)
    {
        if(Datos[i]=="")
        {
          Datos[i]=frace;
          i=100;
        }
    }
    texto = texto.substring(pos+1, texto.length());
  }
  
  for (int i = 0; i < 5; i++)
  {
      if(Datos[i]=="")
      {
        Datos[i]=texto;
        i=100;
      }
  }

}
