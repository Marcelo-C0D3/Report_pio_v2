extern "C"
{
#include "user_interface.h"
}

os_timer_t mTimer1; //timer 1.
os_timer_t mTimer2; //timer 2.
os_timer_t mTimer3; //timer 3.

bool _timeout1 = false;
bool _timeout2 = false;
bool _timeout3 = false;

int T = 15000;
int chargingT = 5000;

void tCallback1(void *tCall)
{
  _timeout1 = true;
}

void tCallback2(void *tCall)
{
  _timeout2 = true;
}

void tCallback3(void *tCall)
{
  _timeout3 = true;
}

void usrInit(void)
{
  os_timer_setfn(&mTimer1, tCallback1, NULL);
  os_timer_setfn(&mTimer2, tCallback2, NULL);
  os_timer_setfn(&mTimer3, tCallback3, NULL);
  os_timer_arm(&mTimer1, T, true);
  os_timer_arm(&mTimer2, T, true);
  os_timer_arm(&mTimer3, chargingT, true);
}

void reset_timer1(void)
{
  os_timer_arm(&mTimer1, T, true);
}
void reset_timer2(void)
{
  os_timer_arm(&mTimer2, T, true);
}

class Manager
{
private:
  const int altOnda = 33;
  const String tittleON = "Som Maior - FM - 100.7 - information Radio: ON";
  const String tittleOFF = "Som Maior - FM - 100.7 - information Radio: OFF";
  const String tittleReport = "Relatorio Radio 100.7 - 24Hrs";
  const String relatControl = "Relatorio de quedas: <br /> ";

public:
  int valor_AD = 0;
  int soma1 = 0;
  int i = 0;
  int x = 0;
  double med = 0;
  double media1 = 0;
  int state = 0;
  int idRelat = 0;
  int hReport = 13;
  int soma = 0;

  String assunto = " ";
  String relatorio = "Relatorio de quedas: <br /> ";
  String queda = " ";
  String day = " ";
  String hours = " ";
  String minutes = " ";
  String seconds = " ";

  void callMedia();
  void report();
  void timerReport();
  void sending(String subject, String e_Mail, String space);
};