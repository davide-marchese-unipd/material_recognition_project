#define IND 2
#define PROX 3
#define LASER 7
#define IR A0
#define LDR A7

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
  // sostituire col numero di utilizzi in scroll screen
  while(digitalRead(PROX) == HIGH);
}

void LCD_init() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
}

int decision(unsigned short infrared, unsigned short optical, bool inductive) {
  if(optical > 850) return 0;
  usages++;
  if(!inductive) return 1;
  else if(optical <= 100 && infrared < 60) return 2;
  else if(infrared >= 40 && infrared <= 200) return 3;
  else if(optical > 100 && optical < 850 && infrared > 500) return 4;
  else return 5;

  // ambiguità carta - plastica se si utilizza il sensore ottico per esaminare la plastica
}

void setup() {
  pinMode(LASER, OUTPUT);
  pinMode(IND, INPUT);
  pinMode(PROX, INPUT);
  Serial.begin(9600);
  LCD_init();
}

void loop() {
    waiting();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("inserire oggetto");
    delay(5000);

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
    delay(5000);
}
