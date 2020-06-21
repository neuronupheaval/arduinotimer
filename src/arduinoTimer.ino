/*
  Timer usando Arduino e DF Robot (ou D1 Robot, se for o produto chinês) 
  LCD Keypad Shield
  
  Autor: Marcelo Shiniti Uchimura <rb tod moc tod lou ta m tod ihcu> 

  Data: 20/06/2020

  Este software tem licença CC BY 4.0, tal como descrito em:
  https://creativecommons.org/licenses/by/4.0/

  Materiais necessários:

  * Arduino Duemilanove, Diecimila ou Uno
  * Shield DF Robot (ou D1 Robot) LCD Keypad
  * Buzzer ativo de 5 volts
  * Fonte de energia para o Arduino ou alimentação por USB

  Instruções:

  Encaixe e conjugue o Arduino ao shield.

  Ajuste o trimpot do contraste do shield para ver os caracteres. O trimpot é
  um troço azul no canto superior esquerdo do shield, com um parafuso bem pe-
  queno de latão, que é o seu cursor.
  
  Eu tive que girar umas boas 9 a 10 voltas até começar a enxergar o display!

  Conecte a fonte de alimentação ao conjugado.

  Funcionamento:

  Existem basicamente três fases de operação do timer:

  1) Modo de ajuste: neste modo, você tem que usar o keypad do shield para
                     ajustar o cronômetro. O cursor mostra qual valor será
                     alterado. Tecla Up para aumentar e tecla Down para di-
                     minuir o valor. Tecla Left para navegar para a esquer-
                     da (segundos, minutos, horas) e tecla Right para nave-
                     gar para a direita (horas, minutos, segundos). Tecla
                     Select inicia o segundo modo, explicado abaixo.

  2) Modo de timer:  uma vez que você tenha apertado a tecla Select na fase
                     anterior, inicia-se a contagem regressiva do timer.
                     Quando o timer atingir 0h 0m 0s, passa-se para o pró-
                     ximo modo.

  3) Modo de alerta: quando o timer expirar, será exibida na tela uma mensa-
                     gem "Timer expirado" e o buzzer deverá apitar. O buzzer
                     deve ser ligado no pino 3 do Arduino (ponta vermelha ou
                     +) e ao GND (ponta preta ou -) e deve ser um buzzer ati-
                     vo de 5 volts.

  A qualquer momento, aperte a tecla Reset para voltar ao modo 1.
*/

#include <LiquidCrystal.h>

#define LCD_TIMEOUT 1000UL
#define KEY_TIMEOUT  160UL

#define SET_CLOCK_H   0
#define SET_CLOCK_M   1
#define SET_CLOCK_S   2
#define RUN_CLOCK     3
#define CLOCK_EXPIRED 4

#define BUZZER_PIN 3

#define KEY_UP     'U'
#define KEY_DOWN   'D'
#define KEY_LEFT   'L'
#define KEY_RIGHT  'R'
#define KEY_SELECT 'S'

#define HOURS   0
#define MINUTES 1
#define SECONDS 2

// Pinos padrão para o shield.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

unsigned long lcdRefresh = 0UL;
unsigned long keyRefresh = 0UL;

int ledStatus = LOW;
int clockState = SET_CLOCK_S;

int digits[] = { 0, 0, 0 };
int ovrflw[] = { 3, 60, 60 };

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(F(" Ajuste o timer "));
  lcd.setCursor(0, 1);
  lcd.print(F("0h 0m 0s"));
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  switch (clockState) {
    case RUN_CLOCK:
      runClock();
      break;
      
    case CLOCK_EXPIRED:
      buzz();
      while (true); // Prende a execução do programa.
      
    default:
      setClock();
      break;
  }
}

void runClock() {
  if (millis() - lcdRefresh < LCD_TIMEOUT) {
    return;
  }

  lcdRefresh = millis();
  decrementClock();
  if (clockState == RUN_CLOCK) displayClock();
}

char keyPressed() {
  int value = analogRead(A0);
  if (value < 60) return KEY_RIGHT;
  if (value < 200) return KEY_UP;
  if (value < 400) return KEY_DOWN;
  if (value < 600) return KEY_LEFT;
  if (value < 800) return KEY_SELECT;
  return 0;
}

void setClock() {
  if (millis() - keyRefresh < KEY_TIMEOUT) {
    return;
  }
  
  keyRefresh = millis();
  char key = keyPressed();
  int hourMinOrSec = clockState;

  switch (key) {
    case KEY_UP:
      if (++digits[hourMinOrSec] >= ovrflw[hourMinOrSec]) {
        digits[hourMinOrSec] = 0;
      }
      break;
      
    case KEY_DOWN:
      if (--digits[hourMinOrSec] < 0) {
        digits[hourMinOrSec] = ovrflw[hourMinOrSec] - 1;
      }
      break;
      
    case KEY_LEFT:
      if (--clockState < SET_CLOCK_H) clockState = SET_CLOCK_S;
      break;
      
    case KEY_RIGHT:
      if (++clockState > SET_CLOCK_S) clockState = SET_CLOCK_H;
      break;
      
    case KEY_SELECT:
      if (digits[SECONDS] != 0 || digits[MINUTES] != 0 || digits[HOURS] != 0) { 
        clockState = RUN_CLOCK;
      }
      break;
  }

  displayClock();
  lcd.noCursor();

  int m = 3 + (digits[MINUTES] > 9 ? 1 : 0);
  int s = m + 3 + (digits[SECONDS] > 9 ? 1 : 0);
  
  switch (clockState) {
    case SET_CLOCK_H:
      lcd.setCursor(0, 1);
      break;
    
    case SET_CLOCK_M:
      lcd.setCursor(m, 1);
      break;
    
    case SET_CLOCK_S:
      lcd.setCursor(s, 1);
      break;
  }

  if (clockState != RUN_CLOCK) lcd.cursor();
}

void displayClock() {
  if (clockState == RUN_CLOCK) {
    displayBanner();
  }
  
  lcd.setCursor(0, 1);
  if (digits[HOURS] != 0 || clockState != RUN_CLOCK) {
    lcd.print(digits[HOURS]);
    lcd.print(F("h "));
  }
  
  if (digits[MINUTES] != 0 || clockState != RUN_CLOCK) {
    lcd.print(digits[MINUTES]);
    lcd.print(F("m "));
  }
  
  if (digits[SECONDS] != 0 || clockState != RUN_CLOCK) {
    lcd.print(digits[SECONDS]);
    lcd.print(F("s"));
  }

  lcd.print(F("      "));
  
  digitalWrite(LED_BUILTIN, 
    ledStatus = ledStatus == HIGH ? LOW : HIGH);
}

void decrementClock() {
  if (--digits[SECONDS] < 0) {
    digits[SECONDS] = ovrflw[SECONDS] - 1;
    
    if (--digits[MINUTES] < 0) {
      digits[MINUTES] = ovrflw[MINUTES] - 1;
      
      if (--digits[HOURS] < 0) {
        clockState = CLOCK_EXPIRED;
      }
    }
  }
}

void buzz() {
  lcd.setCursor(0, 0);
  lcd.print(F(" Timer expirado "));
  digitalWrite(BUZZER_PIN, HIGH);
}

void displayBanner() {
  static int start = 0;
  char bannerString[] = "               Cronometrando               ";
  int bannerLength = 43;

  char banner[17];
  strncpy(banner, bannerString + start, 16);
  banner[16] = '\0';

  lcd.setCursor(0, 0);
  lcd.print(banner);

  if (++start >= bannerLength - 16) {
    start = 0;
  }
}
