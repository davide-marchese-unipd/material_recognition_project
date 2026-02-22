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

unsigned short readAverage(int pin) {
  int sum = 0;
  for(int i=0; i<20; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return sum / 20;
}

void print_msg(char str[][17]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str[0]);
  lcd.setCursor(0, 1);
  lcd.print(str[1]);
}



void waiting() {

  // aggiungere il numero di riconoscimenti per categoria

  long long last_time = millis();
  bool swtch = false;
  char msg1[][17] = {"avvicina la mano", "per iniziare"};
  char msg2[][17] = {"utilizzi:", ""};
  char msg3[][17]= {"", ""};
  char buffer[17] = "";
  itoa(usages, msg2[1], 17);
  print_msg(msg1);
  while(digitalRead(PROX) == HIGH) {
    if(millis() - last_time >= 5000) {
      if(swtch) print_msg(msg1);
      else print_msg(msg2);
      swtch = !swtch;
      last_time = millis();
    }
  }
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
  else if(infrared >= 40 && infrared <= 400) return 3;
  else if(optical > 100) return 4;
  else return 5;
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

    ir_sig = readAverage(IR);
    delay(100);
    digitalWrite(LASER, HIGH);
    delay(1000);
    opt_sig = readAverage(LDR);
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
