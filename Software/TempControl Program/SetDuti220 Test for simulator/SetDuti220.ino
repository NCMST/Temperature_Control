#define OFFSET 1000
uint32_t times = 0;
uint32_t count = 0;

// void counter(void) {
//   count++;
//   digitalWrite(13, !digitalRead(13));
// }

void setup() {
  pinMode(PD2, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(A0, OUTPUT);
  // attachInterrupt(digitalPinToInterrupt(PD2), counter, CHANGE);
  Serial.begin(9600);
  times = millis();
}

void loop() {
  if (analogRead(A0) > 800) {
    count++;
    digitalWrite(13, !digitalRead(13));
    delay(20);
  }
  if (millis() - times > OFFSET) {
    Serial.println(count);
    count = 0;
    times = millis();
  }
}
