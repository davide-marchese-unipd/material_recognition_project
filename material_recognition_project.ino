#define IND 2
#define PROX 3
#define LASER 7
#define IR A0
#define LDR A7

#define LCD_ADDR 0x27

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

unsigned short opt_sig = 0;
unsigned short min_opt_sig = 0;
unsigned short max_opt_sig = 0;
unsigned short ir_sig = 0;
unsigned short min_ir_sig = 0;
unsigned short max_ir_sig = 0;
bool ind_sig = 0;


unsigned int usages = 0;
char materials[][17] = {
  "nulla",
  "metallo",
  "carta",
  "plastica",
  "vetro",
  "indefinito"
};

//si usa per la normalizzazione nello score (formule in foto telefono)
struct material
{
    unsigned short optical_mean;
    unsigned short optical_range;
    unsigned short ir_mean;
    unsigned short ir_range;
    float score;
};

material paper = {38, 49, 66, 9, 0.0};
material plastic = {564, 573, 163, 173, 0.0};
material glass = {406, 504, 752, 232, 0.0};

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

void initialization() {
  /*
    chiudi vano
  */
  max_ir_sig = readAverage(IR); // <- porre attenzione al fatto che il valore per il vetro risulterà non normalizzato dato che sarà superiore al valore massimo teorico
  digitalWrite(LASER, LOW);
  delay(100);
  min_opt_sig = readAverage(LDR);
  digitalWrite(LASER, HIGH);
  delay(100);
  max_opt_sig = readAverage(LDR);
  digitalWrite(LASER, LOW);
}

void waiting() {

  // aggiungere il numero di riconoscimenti per categoria tra i messaggi che scorrono

  long long last_time = millis();
  bool swtch = false;
  char msg1[][17] = {"avvicina la mano", "per iniziare"};
  char msg2[][17] = {"utilizzi:", ""};
  char msg3[][17]= {"", ""};
  char buffer[17] = "";
  itoa(usages, msg2[1], 10);
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

unsigned short normalize(unsigned short sig, unsigned short min_sig, unsigned short max_sig) {
  return (sig - min_sig) / (max_sig - min_sig);
}

void LCD_init() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
}


// int response_analysis(unsigned short infrared, unsigned short optical, bool inductive) {
//   if(optical > 0.95 * max_opt_sig) return 0;
//   usages++;
//   if(!inductive) return 1;

//   else if(infrared > 0.7 * max_ir_sig) return 4;
//   else if(optical <= 100 && infrared < 90) return 2;
//   else if(infrared > 60 && infrared <= 400) return 3;
//   else return 5;
// }

// con score e forse confidenza
// il punteggio non deve andare sotto 0 o sopra 1, in questi casi si deve saturare agli estremi

int score_classification(unsigned short infrared, unsigned short optical, bool inductive) {
// chat "Miglioramenti progetto Arduino" e "Formula per classificazione materiali", devo usare la gaussiana linearizzata e pesare ogni sensore nella formula per ogni materiale
  
}

int response_analysis(unsigned short infrared, unsigned short optical, bool inductive) {
  if(optical > 0.95 * max_opt_sig) return 0;                        // nulla
  usages++;
  if(!inductive) return 1;                                          // metallo
  else if(infrared > 0.7 * max_ir_sig) return 4;                    // vetro
  // else if(optical <= 100 && infrared < 90) return 2;    // carta
  // else if(infrared > 60 && infrared <= 400) return 3;   // plastica
  else return score_classification(infrared, optical, inductive);   // carta / plastica
}

void setup() {
  pinMode(LASER, OUTPUT);
  pinMode(IND, INPUT);
  pinMode(PROX, INPUT);
  Serial.begin(9600);
  initialization();
  Serial.println(min_opt_sig);
  Serial.println(max_opt_sig);
  Serial.println(min_ir_sig);
  Serial.println(max_ir_sig);
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

  int dec = response_analysis(ir_sig, opt_sig, ind_sig);

  // char msg[17] = {"riconosciuto:", *materials[dec]};
  // print_msg(msg);

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

  /*
    muovi pannelli
  */

  delay(5000);
}
