#define IND 2
#define PROX 3
#define LASER 7
#define LDR A7
#define IR A0
#define LCD_ADDR 0x27

#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

unsigned short opt_sig = 0;
unsigned short ir_sig = 0;
bool ind_sig = 0;

unsigned int usages = 0;
char materials[][11] = {
  "nulla",
  "metallo",
  "carta",
  "plastica",
  "vetro",
  "indefinito"
};

void waiting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("presentare");
  lcd.setCursor(0, 1);
  lcd.print("oggetto");
  while(digitalRead(PROX) == HIGH);
}

void LCD_init() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
}

int decision(unsigned short infrared, unsigned short optical, bool inductive) {
  // se ottico > 850 -> non c'è nulla

  // se induttivo è falso -> è metallo

  // se induttivo è vero:

  // se ottico <= 50 & infrarosso < 40 -> è carta

  // se infrarosso in [40, 200) & ottico in (80, 210] -> è plastica

  // se ottico in (210, 850) & infrarosso > 500 -> è vetro

  if(optical > 850) return 0;
  else if(!inductive) return 1;
  else if(optical <= 50 && infrared < 40) return 2;
  else if(optical > 80 && optical <= 230 && infrared >= 40 && infrared <= 200) return 3;
  else if(optical > 230 && optical < 850 && infrared > 500) return 4;
  else return 5;
}

void setup() {
  pinMode(LASER, OUTPUT);
  pinMode(IND, INPUT);
  pinMode(PROX, INPUT);
  Serial.begin(9600);
  LCD_init();
}

// void loop() {
//   digitalWrite(LASER, HIGH);
// }

void loop() {
    waiting();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("inserire oggetto");
    delay(10000);

    ir_sig = analogRead(IR);
    delay(100);
    digitalWrite(LASER, HIGH);
    delay(1000);
    opt_sig = analogRead(LDR);
    delay(100);
    digitalWrite(LASER, LOW);
    ind_sig = digitalRead(IND);

    int dec = decision(ir_sig, opt_sig, ind_sig);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("riconosciuto:");
    lcd.setCursor(0, 1);
    lcd.print(materials[dec]);

    Serial.print("LDR: ");
    Serial.print(opt_sig);
    Serial.print("\tIR: ");
    Serial.print(ir_sig);
    Serial.print("\tIND: ");
    Serial.print(ind_sig);
    Serial.print("\tclass: ");
    Serial.println(materials[dec]);
    delay(10000);
}
