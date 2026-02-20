#define LASER 7
#define LDR A7
#define IR A0
#define IND 2
void setup() {
  pinMode(LASER, OUTPUT);
  Serial.begin(9600);
}

void loop() {
    digitalWrite(LASER, LOW);
    Serial.print("LDR: ");
    Serial.print(analogRead(LDR));
    Serial.print("\tIR: ");
    Serial.print(analogRead(IR));
    Serial.print("\tIND: ");
    Serial.println(digitalRead(IND));
    delay(1000);
}
