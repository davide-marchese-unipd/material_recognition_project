#define IND 2
#define PROX 3
#define LASER 7
#define IR A0
#define LDR A7
#define UPPER_SERVO 8
#define LOWER_SERVO 9
#define CENTRAL_SERVO 10
#define LEFT_SERVO 11
#define RIGHT_SERVO 12

#define LCD_ADDR 0x27

#include <LiquidCrystal_I2C.h>
#include <Servo.h>

Servo u_servo;
Servo d_servo;
Servo c_servo;
Servo l_servo;
Servo r_servo;

unsigned short u_servo_rest = 5;
unsigned short d_servo_rest = 80;
unsigned short c_servo_rest = 95;
unsigned short l_servo_rest = 95;
unsigned short r_servo_rest = 100;

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

struct material {
    unsigned short opt_mean;
    unsigned short opt_range;
    unsigned short ir_mean;
    unsigned short ir_range;
    float score;
};


material materials_data[] = {
  {38, 49, 66, 9, 0.0},                     // carta
  {564, 573, 163, 173, 0.0},                // plastica
  {406, 504, 752, 232, 0.0}                 // vetro
};

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

//
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

void servo_reset() {
  for(int i = u_servo_rest + 10; i > u_servo_rest; i--) {
    u_servo.write(i);
    delay(15);
  }
  for(int i = c_servo_rest - 10; i < c_servo_rest; i++) {
    c_servo.write(i);
    delay(15);
  }
  for(int i = l_servo_rest - 10; i < l_servo_rest; i++) {
    l_servo.write(i);
    delay(15);
  }
  for(int i = r_servo_rest - 10; i < r_servo_rest; i++) {
    r_servo.write(i);
    delay(15);
  }
}

void servo_move(int decision) {
  for(int i = 90; i >= 0; i--) {
    d_servo.write(i);
    delay(10);
  }
  for(int i = 0; i <= 90; i++) {
    d_servo.write(i);
    delay(10);
  }
  switch(decision) {
    case 0:
      break;
    case 1:
      for(int i = l_servo_rest; i <= 110; i++) l_servo.write(i);
      for(int i = c_servo_rest; i <= 120; i++) c_servo.write(i);
      break;
    case 2:
      for(int i = r_servo_rest; i <= 180; i++) r_servo.write(i);
      for(int i = c_servo_rest; i >= 35; i--) c_servo.write(i);
      break;
    case 3:
      for(int i = l_servo_rest; i >= 15; i--) l_servo.write(i);
      for(int i = c_servo_rest; i <= 160; i++) c_servo.write(i);
      break;
    case 4:
      for(int i = r_servo_rest; i >= 80; i--) r_servo.write(i);
      for(int i = c_servo_rest; i >= 70; i--) c_servo.write(i);
      break;
  }
  delay(50);
}

//
void initialization() {
  LCD_init();
  max_ir_sig = readAverage(IR);
  digitalWrite(LASER, LOW);
  delay(100);
  min_opt_sig = readAverage(LDR);
  digitalWrite(LASER, HIGH);
  delay(100);
  max_opt_sig = readAverage(LDR);
  digitalWrite(LASER, LOW);
}

//
void waiting() {
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

//
unsigned short normalize(unsigned short sig, unsigned short min_sig, unsigned short max_sig) {
  return (sig - min_sig) / (max_sig - min_sig);
}

//
void LCD_init() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
}

//
float compute_score(float value, float mean, float range) {
    float distance = fabs(value - mean);
    if(distance >= range)
        return 0.0;
    return 1.0 - (distance / range);
}

//
int score_classification(unsigned short infrared, unsigned short optical) {
  for(int i = 0; i < 2; i++) {
    materials_data[i].score = compute_score(infrared, materials_data[i].ir_mean, materials_data[i].ir_range);
    materials_data[i].score += compute_score(optical, materials_data[i].opt_mean, materials_data[i].opt_range);
    materials_data[i].score /= 2;
  }

  if(materials_data[0].score > materials_data[1].score) {
    Serial.print("score: ");
    Serial.println(materials_data[0].score);
    return 2;
  }      // carta
  else if(materials_data[1].score > materials_data[0].score) {
    Serial.print("score: ");
    Serial.println(materials_data[1].score);
    return 3;
  } // plastica

  return 5; // non identificato
}

//
int response_analysis(unsigned short infrared, unsigned short optical, bool inductive) {
  if(optical > 0.95 * max_opt_sig && infrared > 0.95 * max_ir_sig) return 0;                        // nulla
  usages++;
  if(!inductive) return 1;                                          // metallo
  if(infrared > 0.7 * max_ir_sig) return 4;              //vetro
  else return score_classification(infrared, optical);   // carta / plastica
}

void setup() {
  Serial.begin(9600);
  pinMode(LASER, OUTPUT);
  pinMode(IND, INPUT);
  pinMode(PROX, INPUT);
  u_servo.attach(UPPER_SERVO);
  d_servo.attach(LOWER_SERVO);
  c_servo.attach(CENTRAL_SERVO);
  l_servo.attach(LEFT_SERVO);
  r_servo.attach(RIGHT_SERVO);
  initialization();
  Serial.println(min_opt_sig);
  Serial.println(max_opt_sig);
  Serial.println(min_ir_sig);
  Serial.println(max_ir_sig);
  servo_reset();
}

void loop() {
  waiting();
  
  u_servo.write(90);

  char insert[][17] = {"inserire oggetto", ""};
  print_msg(insert);
  delay(10000);
  u_servo.write(u_servo_rest);

  delay(500);

  ir_sig = readAverage(IR);
  delay(100);
  digitalWrite(LASER, HIGH);
  delay(1000);
  opt_sig = readAverage(LDR);
  delay(100);
  digitalWrite(LASER, LOW);
  ind_sig = digitalRead(IND);

  int dec = response_analysis(ir_sig, opt_sig, ind_sig);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("riconosciuto:");
  lcd.setCursor(0, 1);
  lcd.print(materials[dec]);
  
  servo_move(dec);

  Serial.print("LDR: ");
  Serial.print(opt_sig);
  Serial.print("\tIR: ");
  Serial.print(ir_sig);
  Serial.print("\tIND: ");
  Serial.print(ind_sig);
  Serial.print("\tclass: ");
  Serial.println(materials[dec]);

  delay(2500);
  servo_reset();
  delay(2500);
}