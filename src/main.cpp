#include <Arduino.h>

#include <../lib/NTPClient/NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>                      //Local WebServer used to serve the configuration portal
#include <../lib/WiFiManager-master/WiFiManager.h> // WiFi Configuration Magic ( https://github.com/zhouhan0126/WIFIMANAGER-ESP32 ) >> https://github.com/tzapu/WiFiManager (ORIGINAL)
#include <../GmailSender/Gsender.h>                // WiFi Configuration Magic ( https://github.com/zhouhan0126/WIFIMANAGER-ESP32 ) >> https://github.com/tzapu/WiFiManager (ORIGINAL)
#include <main.h>

//flag para indicar se foi salva uma nova configuração de rede
bool shouldSaveConfig = false;
const long utcOffsetInSeconds = -10800;
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabado"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WiFiManager wifiManager;
Manager go;

void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void sender();
void timerReport();

void setup()
{
  Serial.begin(115200);

  wifiManager.autoConnect("ESP_WIFI", "807060503020"); //cria uma rede com senha
  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback);         //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); // save configs
  usrInit();
}

void loop()
{
  go.valor_AD = analogRead(A0);
  go.soma = go.soma + go.valor_AD;
  go.i++;

  if (go.i == 25)
  {
    go.callMedia();
  }
  else
  {
    go.report();
  }
}

//callback que indica que o ESP entrou no modo AP
void configModeCallback(WiFiManager *myWiFiManager)
{
  //  Serial.println("Entered config mode");
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP());                      //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
}
//callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback()
{
  //  Serial.println("Should save config");
  Serial.println("Configuração salva");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
}

void Manager::callMedia()
{

  if (x == 10)
  {
    media1 = soma1 / x;
    x = 0;
    soma1 = 0;
  }
  else if (x == 3 && media1 > altOnda)
  {
    reset_timer2();
    yield();
  }
  else
  {
    med = soma / i;
    soma1 = soma1 + med;
    i = 0;
    x++;
    soma = 0;
  }
}

//function to resports.
void Manager::report()
{
  if (_timeout3)
  {
    timerReport();
    digitalWrite(D5, !digitalRead(D5));
    _timeout3 = false;
  }
  else if (media1 > altOnda && (state == 0 || state == 1))
  {
    reset_timer1();
    state = 3;
  }
  else if (_timeout2 && (state == 0 || state == 2))
  {
    state = 1;
    assunto = "A rádio está fora do ar, ou Sem áudio perceptível.";

    //sending(tittleON,"gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleON,"arthurlessa@gmail.com", assunto);
    sending(tittleOFF, "marceloloch.newaa@gmail.com", assunto);
    reset_timer1();
    _timeout1 = false;

    queda = day + ", " + hours + ":" + minutes + ":" + seconds + "<br />";
    relatorio = relatorio + queda;
  }
  else if (media1 > altOnda && _timeout1 && (state == 0 || state == 3))
  {

    state = 2;
    assunto = "A rádio está ativa e estável.";

    //sending(tittleON,"gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleON,"arthurlessa@gmail.com", assunto);
    sending(tittleON, "marceloloch.newaa@gmail.com", assunto);
    //sender();
    _timeout2 = false;
  }
}

// function to reprot relat in time exact;
void Manager::timerReport()
{

  timeClient.update();
  day = daysOfTheWeek[timeClient.getDay()];
  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  seconds = timeClient.getSeconds();

  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.println();

  if (relatorio != relatControl && timeClient.getMinutes() == hReport && idRelat == 0)
  {
    idRelat = 1;
    assunto = relatorio;
    //sending(tittleON,"gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleON,"arthurlessa@gmail.com", assunto);
    sending(tittleReport, "marceloloch.newaa@gmail.com", assunto);
  }
  else if (relatorio == relatControl && timeClient.getMinutes() == hReport && idRelat == 0)
  {
    idRelat = 1;
    assunto = "A rádio não apresentou nenhuma queda nas ultimas 24H.";
    //sending(tittleON,"gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleON,"arthurlessa@gmail.com", assunto);
    sending(tittleReport, "marceloloch.newaa@gmail.com", assunto);
  }
  else if (timeClient.getHours() == (hReport + 1) && idRelat == 1)
  {
    idRelat = 0;
    relatorio = relatControl;
  }
}

void Manager::sending(String subject, String e_Mail, String space)
{

  if (WiFi.status() != WL_CONNECTED)
  {
    wifiManager.autoConnect("ESP_WIFI", "807060503020");
  }

  Gsender *gsender = Gsender::Instance(); // Getting pointer to class instance
  if (gsender->Subject(subject)->Send(e_Mail, space))
  {
    Serial.println("Message send.");
  }
  else
  {
    Serial.print("Error sending message: ");
    Serial.println(gsender->getError());
  }
}