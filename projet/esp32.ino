#include <WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "Damn";
const char* password = "fuckinggood";

const char* mqtt_server = "172.20.10.8";
const char* mqtt_user = "gong";
const char* mqtt_password = "weiyigong";

WiFiClient wifiClient;

PubSubClient client(mqtt_server,1883,wifiClient);

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


void setup_wifi()
{
  delay(10);
  Serial.println();

  WiFi.begin(ssid,password);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  
}

void connect_rasp()
{

  while(!client.connected())
  {
    if(client.connect("ESP32CLIENT",mqtt_user,mqtt_password))
    {
      Serial.println("Connected with Raspb");
    }
    else
    {
      Serial.print("failed,rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(4,15);

  setup_lum(&lum_publish,0,500000);//Timer 1 et periode 5000000
  
  setup_wifi();
  client.setServer(mqtt_server,1883);
  //client.setCallback();


  
}

void loop() {
  if(!client.connected())
  {
      connect_rasp();
  }
  // put your main code here, to run repeatedly:
  //attachInterrupt(digitalPinToInterrupt(2), serialEvent, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(3), serialEvent, CHANGE);//Charger 2 fonction pour attach l'interrupteurs en boucle chaque fois
  //loop_readLum(&mb_lum,&lum1,&mb_led);
  //loop_afficheLum(&mb_lum);
  //oledCount(&counter1);
  //loop_ledLum(&mb_led,&led,&mb_isr);
  //loop_bp(&bp,&mb_buzzer);
  //loop_buzzer(&buzzer,&mb_buzzer);//Generer les fonctions

  loop_Lum(&mb_publish,&lum_publish);

  if(mb_publish.state != EMPTY)
  {
    String lum = (String)(mb_publish.val);
    client.publish("topic_lum",lum.c_str());
    mb_publish.state = EMPTY;
  }
}
