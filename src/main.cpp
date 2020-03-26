#include <Arduino.h>
#include <../lib/NTPClient/NTPClient.h>            //Network time protocol lib.
#include <ESP8266WiFi.h>                           //Wifi Propierts to esp8266
#include <WiFiUdp.h>                               //Service UDP, time!
#include <ESP8266WebServer.h>                      //Local WebServer used to serve the configuration portal
#include <../lib/WiFiManager-master/WiFiManager.h> // WiFi Configuration Magic ( https://github.com/zhouhan0126/WIFIMANAGER-ESP32 ) >> https://github.com/tzapu/WiFiManager (ORIGINAL)
#include <../GmailSender/Gsender.h>                // Librarie to Sender SMTP gmail.
#include <main.h>                                  //Progam Main Radio_Pio_v2

bool shouldSaveConfig = false;                                                                      //Flag Save  config WiFi
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabado"}; //Dayes of week :D

Manager go; // reference to class main
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", go.utcOffsetInSeconds);
WiFiManager wifiManager;

void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void tCallback1(void *tCall);
void tCallback2(void *tCall);
void tCallback3(void *tCall);

void setup()
{
  Serial.begin(115200);

  Serial.println(go.checkWiFiConect()); //Verifica e connecta ao wifi.
  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback);         //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); // save configs
  go.usrInit();
}

void loop()
{
  if (go.i < 25)
  {
    go.getValues();
  }
  else
  {
    go.callMedia();
  }
}

//callback que indica que o ESP entrou no modo AP
String Manager::checkWiFiConect()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    wifiManager.autoConnect(serveName, servePass);
    return "Conexao Reestabelecida!....";
  }
  else
  {
    return "Conectado!!";
  }
}

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
void Manager::getValues()
{

  valor_AD = analogRead(A0);
  soma = soma + valor_AD;
  i++;
}
void Manager::callMedia()
{

  if (x == 10)
  {
    media1 = soma1 / x;
    x = 0;
    soma1 = 0;
    delay(100);
    go.report();
  }
  else if (x == 3 && media1 > altOnda)
  {
    delay(100);
    reset_timer2();
    yield();
  }
  else
  {
    delay(100);
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

    //sending(tittleOFF,"gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleOFF,"arthurlessa@gmail.com", assunto);
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

  if (relatorio != relatControl && timeClient.getHours() == hReport && idRelat == 0)
  {
    idRelat = 1;
    assunto = relatorio;
    //sending(tittleReport, "gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleReport, "arthurlessa@gmail.com", assunto);
    sending(tittleReport, "marceloloch.newaa@gmail.com", assunto);
  }
  else if (relatorio == relatControl && timeClient.getHours() == hReport && idRelat == 0)
  {
    idRelat = 1;
    assunto = "A rádio não apresentou nenhuma queda nas ultimas 24H.";
    //sending(tittleReport, "gustavo.lucca@satc.edu.br", assunto);
    //sending(tittleReport, "arthurlessa@gmail.com", assunto);
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
  Serial.println(go.checkWiFiConect());   //Verifica e connecta ao wifi.
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

void Manager::usrInit()
{
  os_timer_setfn(&mTimer1,tCallback1, NULL);
  os_timer_arm(&mTimer1, T, true);
  os_timer_setfn(&mTimer2, tCallback2, NULL);
  os_timer_arm(&mTimer2, T, true);
  os_timer_setfn(&mTimer3, tCallback3, NULL);
  os_timer_arm(&mTimer3, chargingT, true);
}

void Manager::reset_timer1()
{
  os_timer_arm(&mTimer1, T, true);
}
void Manager::reset_timer2()
{
  os_timer_arm(&mTimer2, T, true);
}

void tCallback1(void *tCall)
{
  go._timeout1 = true;
}

void tCallback2(void *tCall)
{
  go._timeout2 = true;
}

void tCallback3(void *tCall)
{
  go._timeout3 = true;
}