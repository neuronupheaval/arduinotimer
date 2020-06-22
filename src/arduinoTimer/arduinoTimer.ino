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

#define LCD_COLUMNS 16
#define LCD_LINES    2

#define TRUE  1
#define FALSE 0

// Pinos padrão para o shield.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

unsigned long lcdRefresh = 0UL;
unsigned long keyRefresh = 0UL;

int ledStatus = LOW;
int clockState = SET_CLOCK_M;

int digits[] = { 0, 0, 0 };
int ovrflw[] = { 3, 60, 60 };

char message[] = "Cronometrando";
char banner[4 * LCD_COLUMNS];
const int bannerLength = 2 * (LCD_COLUMNS - 1) + strlen(message) + 1 /*terminado em NULL*/;

void setup() {
  lcd.begin(LCD_COLUMNS, LCD_LINES);
  lcd.setCursor(0, 0);
  lcd.print(F(" Ajuste o timer "));
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Preparação do banner.
  char spaces[(LCD_COLUMNS - 1) /*um char a menos*/ + 1 /*terminado em NULL*/];
  for (int i = 0; i < LCD_COLUMNS - 1; ++i) {
    spaces[i] = ' ';
  }
  spaces[LCD_COLUMNS - 1] = '\0';
  strcpy(banner, spaces);
  strcat(banner, message);
  strcat(banner, spaces);

  // Inicialização.
  int m, s, totalLength, start;
  calculateCoordinates(&m, &s, &totalLength, &start, FALSE);
  displayClock(start);
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
  if (clockState != CLOCK_EXPIRED) {
    int m, s, totalLength, start;
    calculateCoordinates(&m, &s, &totalLength, &start, TRUE);
    displayClock(start);
  }
}

void buzz() {
  lcd.setCursor(0, 0);
  lcd.print(F(" Timer expirado "));
  digitalWrite(BUZZER_PIN, HIGH);
}

void setClock() {
  if (millis() - keyRefresh < KEY_TIMEOUT) {
    return;
  }  
  keyRefresh = millis();

  int idle = processKeypad();

  int m, s, totalLength, start;
  calculateCoordinates(&m, &s, &totalLength, &start, FALSE);

  if (!idle) {
    displayClock(start);
  }
  showCursorIfNeeded(m, s, start);
}

int processKeypad() {
  char key = '\0';
  int idleness = FALSE;
  int hourMinOrSec = clockState;
  int value = analogRead(A0);
  
  if (value < 800) key = KEY_SELECT;
  if (value < 600) key = KEY_LEFT;
  if (value < 400) key = KEY_DOWN;
  if (value < 200) key = KEY_UP;
  if (value < 60)  key = KEY_RIGHT;

  switch (key) {
    case KEY_UP:
      if (++digits[hourMinOrSec] >= ovrflw[hourMinOrSec]) digits[hourMinOrSec] = 0;
      break;
      
    case KEY_DOWN:
      if (--digits[hourMinOrSec] < 0) digits[hourMinOrSec] = ovrflw[hourMinOrSec] - 1;
      break;
      
    case KEY_LEFT:
      if (--clockState < SET_CLOCK_H) clockState = SET_CLOCK_S;
      break;
      
    case KEY_RIGHT:
      if (++clockState > SET_CLOCK_S) clockState = SET_CLOCK_H;
      break;
      
    case KEY_SELECT:
      if (digits[SECONDS] != 0 || digits[MINUTES] != 0 || digits[HOURS] != 0) clockState = RUN_CLOCK;
      break;

    default:
      idleness = TRUE;
      break;
  }

  return idleness;
}

void displayClock(int start) {
  if (clockState == RUN_CLOCK) {
    displayBanner();
  }
  printClockToLcd(start);
}

void showCursorIfNeeded(int m, int s, int start) {
  lcd.noCursor();
  
  if (clockState != RUN_CLOCK) {
    switch (clockState) {
      case SET_CLOCK_H:
        lcd.setCursor(start, 1);
        break;
      
      case SET_CLOCK_M:
        lcd.setCursor(start + m, 1);
        break;
      
      case SET_CLOCK_S:
        lcd.setCursor(start + s, 1);
        break;
    }
    
    lcd.cursor();
  }
}

void printClockToLcd(int start) {
  lcd.setCursor(0, 1);
  lcd.print(F("                "));

  lcd.setCursor(start, 1);
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

  // Pisca o LED da placa Arduino.
  digitalWrite(LED_BUILTIN, 
    ledStatus = ledStatus == HIGH ? LOW : HIGH);
}

void displayBanner() {
  static int start = 0;
    
  char partialBanner[LCD_COLUMNS + 1 /*terminado em NULL*/];
  strncpy(partialBanner, banner + start, LCD_COLUMNS);
  partialBanner[LCD_COLUMNS] = '\0';

  lcd.setCursor(0, 0);
  lcd.print(partialBanner);

  if (++start >= bannerLength - LCD_COLUMNS) {
    start = 0;
  }
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

void calculateCoordinates(int* m, int* s, int* totalLength, int* start, int snipZeros) {
  *m = (snipZeros == TRUE && digits[HOURS] == 0
    ? 0
    : 3 /*uhESPAÇO*/ + (digits[MINUTES] > 9 ? 1/*uhESPAÇOd(um)*/ : 0/*uhESPAÇO(um)*/));
    
  *s = *m + (snipZeros == TRUE && digits[MINUTES] == 0
    ? 0
    : 3 /*umESPAÇO*/ + (digits[SECONDS] > 9 ? 1/*umESPAÇOd(us)*/ : 0/*umESPAÇO(us)*/));
    
  *totalLength = *s + (snipZeros == TRUE && digits[SECONDS] == 0
    ? -1 /*desconta ESPAÇO extra*/
    :  2 /*us*/ + (*s == 0 && digits[SECONDS] > 9 ? 1/*dus*/ : 0/*us*/));
    
  *start = (LCD_COLUMNS - *totalLength) / 2;
}
