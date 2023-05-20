#include <WiFi.h>                 // Library for wifi
#include <PubSubClient.h>         // Library for MQTT communication
  
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "Damn";        // Username of WIFI
const char* password = "fuckinggood";   // Password for WIFI
  
const char* mqtt_server = "172.20.10.8";    // IP of MQTT
const char* mqtt_user = "gong";             // Username of MQTT
const char* mqtt_password = "weiyigong";    // Password of MQTT

WiFiClient wifiClient;                      // Creation of wifi client

PubSubClient client(mqtt_server,1883,wifiClient);   // Create an instance of the PubSubClient class to handle MQTT communication

#define sensor_pin  36

#define MAX_WAIT_FOR_TIMER 5  //Definition du nombre de timer

enum {EMPTY, FULL}; //Definition pour les flags

struct mailbox_s {    //Structure de mailbox
  int state;        //State soit EMPTY, soit FULL
  int val;          //La valeur de mailbox qui a besoin de communiquer entre les different taches
};

struct mailbox_s mb_publish = {.state = EMPTY};     //Initialisation de mailbox pour l'intensite de la lumiere

struct lum{
  int timer;
  unsigned long period;
};

struct lum lum_publish;

unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

void setup_lum(struct lum *lum_publish, int timer, unsigned long period)//Setup de timer pour recuperer l'intensite de la lumiere
{
  lum_publish->timer = timer;
  lum_publish->period = period;
}

int readLum()                                            // La fonction pour recupere l'intensite de la lumiere
{
  int lum = analogRead(sensor_pin);                        // Lire la valeur dans le pin de sensor
  lum = map(lum, 0, 4096, 100, 0);
  return lum;
}

void loop_Lum(struct mailbox_s *mb_publish, struct lum *lum_publish) {
  if (!(waitFor(lum_publish->timer,lum_publish->period))) return;  //Si le temps n'est pas passe une periode
  int lumvalue = readLum();
  
  if (mb_publish->state != FULL) {
    mb_publish->val = lumvalue;    // On peut directement envoyer la valeur vers oled
    mb_publish->state = FULL; // Laisse le flag de mailbox en FULL
  }
}


void setup_wifi()           // WIFI setup
{
  delay(10);
  Serial.println();

  WiFi.begin(ssid,password);  // Conncect with WIFI
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED)    // While waiting the connection of the WIFI
  {
    Serial.print(".");
    delay(100);
  }
// After connection
  Serial.println("\nConnected to WiFi network");    
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  
}

void connect_rasp()   // Connect with raspberry mqtt mosquitto
{

  while(!client.connected())
  {
    if(client.connect("ESP32CLIENT",mqtt_user,mqtt_password))   // Connection of mqtt
    {
      Serial.println("Connected with Raspb");
      //client.subscribe("topic_buzzer",mqtt_user,mqtt_password);
      //client.subscribe("topic_led",mqtt_user,mqtt_password);
    }
    else
    {
      Serial.print("failed,rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

char buf[50];

void callback(const char* topic,byte* payload,unsigned int length)
{
  String Topic = String(topic);
  Serial.println(Topic);
  Serial.print("length message received in callback");
  Serial.println(length);
  int i;
  for(i = 0; i < length; i++)
  {
    buf[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
  buf[i] = '\0';
  String str = String(buf);

  if(Topic == "topic_buzzer")
  {

    
  }
  else if(Topic == "topic_led")
  {

    
  }

  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(4,15);

  setup_lum(&lum_publish,0,500000);//Timer 1 et periode 5000000
  
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);


  
}

void loop() {
  if(!client.connected())   // if the mqtt is not connected
  {
      connect_rasp();       // reconnect the mqtt
  }

  loop_Lum(&mb_publish,&lum_publish);   // Loop for read the luminosity

  if(mb_publish.state != EMPTY)
  {
    String lum = (String)(mb_publish.val);    // Convert the value to str for the message send
    client.publish("topic_lum",lum.c_str());  // Send the message to the mqtt
    mb_publish.state = EMPTY;
  }
}
