#include "user_interface.h"

class Manager
{
private:
  const int altOnda = 35;
  const int hReport = 13;
  const String tittleON = "Som Maior - FM - 100.7 - information Radio: ON";
  const String tittleOFF = "Som Maior - FM - 100.7 - information Radio: OFF";
  const String tittleReport = "Relatorio Radio 100.7 - 24Hrs";
  const String relatControl = "Relatorio de quedas: <br /> ";
  const char *serveName = "Radio_Monitor";
  const char *servePass = "s3nh4";
  int T = 20000;
  int chargingT = 25000;

public:
  int valor_AD = 0, soma1 = 0, i = 0, x = 0, state = 0, idRelat = 0, soma = 0;
  double med = 0, media1 = 0;
  const long utcOffsetInSeconds = -10800; //Ajuste fusu-horario BR SC
  
  bool _timeout1 = false, _timeout2 = false, _timeout3 = false;

  String assunto = " ";
  String relatorio = "Relatorio de quedas: <br /> ";
  String queda = " ";
  String day = " ";
  String hours = " ";
  String minutes = " ";
  String seconds = " ";

  os_timer_t mTimer1, mTimer2, mTimer3; //timer 1./timer 2./timer 3.

  String checkWiFiConect();
  void getValues();
  void callMedia();
  void report();
  void timerReport();
  void sending(String subject, String e_Mail, String space);

  void usrInit();
  void reset_timer1();
  void reset_timer2();
};