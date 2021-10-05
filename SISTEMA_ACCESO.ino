#include <WiFi.h>
#include <MFRC522.h> 
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32_Servo.h> 
//-----------------------SERVO-------------------------
Servo myservo;
int servoPin = 12;
//-----------------------MODULO LCD--------------------
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
byte N[8] = {
B11111,
B11111,
B11111,
B11111,
B11111,
B11111,
B11111,
B11111,
};

//-----------------------MODULO WIFI-------------------
const char* ssid = ""; //Nombre de la red wifi
const char* password = ""; //Contraseña wifi
WiFiClient client;


//---------------------MODULO PHP----------------------
char server[] = "";//Dirección IP del servidor con la página PHP


//---------------------MODULO RFID---------------------
constexpr uint8_t RST_PIN = 13;  
constexpr uint8_t SS_PIN = 5;   
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

char c;
byte ActualUID[4]; //almacenará el código del Tag leído


//---------------------OTRAS VARIABLES------------------
int estado=1;
int dato_1=0;
bool cicloWhile1=true;
String codigo;        //Aquí se almacena la respuesta del servidor
boolean fin = false;
String nombre;        //Aquí se almacena el nombre que recuperamos de MySQL
String identificador;
String identificador1;
byte UID[4]; //almacenará el código del Tag leído
String placa="";
String peticion;
String IDusuario="";
bool userAcceso;
bool acceso;
//---------------------
char picoPlacaM[] ="MXX5XX";//Pico y placa moto
char picoPlacaC[] ="CXXXX3";//Pico y placa carro
char picoPlacaA[] ="AXXXXXX";//administrador

//.........................................................................
void setup() {
    Serial.begin(9600); //Iniciar puerto serie
    //------------------INICIO SERVO- -------------------
      myservo.attach(servoPin,100, 2800);
    //------------------INICIO LCD- -------------------
    lcd.init();                     
    lcd.backlight();
    lcd.setCursor(0, 1);
    lcd.print("Iniciando Wifi...");
    //------------------INICIO RFID -------------------
    SPI.begin(); 
    mfrc522.PCD_Init(); 
    //------------------INICIO CONEXIÓN WIFI-----------
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //------------------------------
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Conectado a red WIFI");
    delay(3000);
    lcd.clear();
    moduloLCDsms1(); //mensaje bienvenida

    //------------------INICIO VARIABLES-----------
}
//.........................................................................
void loop() {
      switch (estado) {
            //---------------------------------
            case 1://INICIO
            Serial.println("1. INICIO");
            cicloWhile1=true;
            moduloLCDsms2(); //mensaje de acceso
            moduloRFID();
            break;
            //---------------------------------
            case 2://LECTURA
            Serial.println("2. LECTURA");
            //identificador="137165183178";
            placa=httpRequest(identificador);
            peticion=""; //reiniciar variable
            moduloLCDsms3(); //Mensaje de petición
            break;
            //---------------------------------
            case 3://PETICION
            Serial.println("3. PETICION");
            userAcceso=UserSolicitud(placa);
            placa=""; //reiniciar variable
            if(acceso==true){
              estado=4;
            }
            else{
              estado=5;
            }
            acceso=false;
            break;
            //---------------------------------
            case 4: // ACCESO AUTORIZADO
            Serial.println("4. ACCESO AUTORIZADO");
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("AUTORIZADO");
            lcd.setCursor(0, 1);
            lcd.print("Cerrando en:");
            Serial.println("Autorizado");
            //............................
            myservo.write(10);  
            delay(100);
            myservo.write(100);
            delay(1000); 
            //............................
             lcd.createChar (7,N);
              for(int i=0; i<=19; i++){
                int count=19-i;
                lcd.setCursor(i, 2);
                lcd.write (byte (7));
                lcd.setCursor(13, 1);
                lcd.print(count);
                delay(1000); 
              }
            //............................
            myservo.write(10);  
            delay(1000); 
            //............................
            estado=1;
            break;
            //---------------------------------
            default: // ACCESO NO AUTORIZADO
            Serial.println("4. ACCESO NO AUTORIZADO");
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("NO AUTORIZADO");
            Serial.println("No Autorizado");
            delay(1000);
            estado=1;            
            break;
            //---------------------------------
      }
}
//.........................................................................
void moduloRFID() {
  // Revisamos si hay nuevas tarjetas  presentes
  while(cicloWhile1==true)
  if(mfrc522.PICC_IsNewCardPresent()) 
        { 
          Serial.println("dentro del while"); 
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.println("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          UID[i]=mfrc522.uid.uidByte[i];
                  } 
                  String myString1 = String(UID[0]);
                  String myString2 = String(UID[1]);
                  String myString3 = String(UID[2]);
                  String myString4 = String(UID[3]);
                  identificador=myString1+myString2+myString3+myString4;
                  Serial.println(identificador);
                  mfrc522.PICC_HaltA(); 
                  //----------------------------------------
                   estado=2;
                   cicloWhile1=false;
                  //---------------------------------------
            }      
  }
  else{
    cicloWhile1=true;
  }
  
}
//.........................................................................
void moduloLCDsms1(){
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Bienvenidos");
  lcd.setCursor(0, 1);
  lcd.print("Cargando....");
  lcd.createChar (7,N);
  for(int i=0; i<=19; i++){
      lcd.setCursor(i, 2);
      lcd.write (byte (7));
      delay(300); 
  }
}
//.........................................................................
void moduloLCDsms2(){
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("USUARIO UCO");
  lcd.setCursor(0, 2);
  lcd.print("Colocar el carnet");
  lcd.setCursor(0, 3);
  lcd.print("Sobre el lector");
}
//.........................................................................
void moduloLCDsms3(){
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Accediendo"); 
  estado=3;
}
//.........................................................................
// Con esta función hacemos la conexión con el servidor
String httpRequest(String identificador) {
            // Comprobar si hay conexión
            if (client.connect(server, 80)) {
                    Serial.println("Conectado");
                    // Enviar la petición HTTP
                    //Dirección del archivo php dentro del servidor
                    client.print("GET http://localhost/acceso/datosmysql.php?id_carnet=");
                    //Mandamos la variable junto a la línea de GET
                    client.print(identificador);
                    client.println(" HTTP/1.1");
                    //IP del servidor
                    client.println("Host: ");
                    client.println("Connection: close");
                    client.println();
            }
            else {
                    // Si no conseguimos conectarnos
                    Serial.println("Conexión fallida");
                    Serial.println("Desconectando");
                    client.stop();
            }
            delay(500);
            //Comprobamos si tenemos respuesta del servidor y la
            //almacenamos en el string ----> codigo.
            while (client.available()) {
                    char c = client.read();
                    codigo += c;
                    //Habilitamos la comprobación del código recibido
                    fin = true;
            }
            //Si está habilitada la comprobación del código entramos en el IF
            if (fin)  {
                    // Serial.println(codigo);
                    //Analizamos la longitud del código recibido
                    int longitud = codigo.length();
                    //Buscamos en que posición del string se encuentra nuestra variable
                    int posicion = codigo.indexOf("placa=");
                    //Borramos lo que haya almacenado en el string nombre
                    nombre = "";
                    //Analizamos el código obtenido y almacenamos el nombre en el string nombre
                    for (int i = posicion + 6; i < longitud; i ++){
                        if (codigo[i] == ';') i = longitud;
                        else nombre += codigo[i];
                    }
                    //Deshabilitamos el análisis del código
                    fin = false;
                    //Imprimir el nombre obtenido
                    //---------------------------------------------------------------------------------
                    peticion=nombre;
                    Serial.print("Mysql: ");
                    Serial.println(peticion);
                    //---------------------------------------------------------------------------------
                    Serial.println("Valor de la variable nombre: " + nombre);
                    //Cerrar conexión
                    Serial.println("Desconectar");
                    client.stop();
            }
            //Borrar código y salir de la función//Dirección IP del servidor
            codigo="";
            return peticion;
}
//-----------------------------------------------------------------------------------------------------
bool UserSolicitud(String placa){
  int str_len = placa.length() + 1;
  char placaArray[str_len]; 
  placa.toCharArray(placaArray, str_len);
  Serial.println("Placa base de datos:");
  Serial.print(placaArray[1]);
  Serial.print(placaArray[2]);
  Serial.print(placaArray[3]);
  Serial.print(placaArray[4]);
  Serial.print(placaArray[5]);
  Serial.print(placaArray[6]);
  Serial.println(placaArray[7]);
  //----------------------------------
  if(placaArray[1]==picoPlacaM[0]){
    Serial.println("Moto");
      if(placaArray[5]==picoPlacaM[3]){
       acceso=false; 
      }
      else{acceso=true; }
  }
  //----------------------------------
  if(placaArray[1]==picoPlacaC[0]){
    Serial.println("Carro");
      if(placaArray[7]==picoPlacaC[5]){
       acceso=false; 
      }
      else{acceso=true; }
  }
  //----------------------------------
  if(placaArray[1]==picoPlacaA[0]){
    Serial.println("Administrador");
    acceso=true;
  }
  //----------------------------------
  return acceso;
}
