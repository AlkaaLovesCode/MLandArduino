#include <Arduino.h>
#include <Servo.h>

Servo servo;
int angle = 90;

void setup() {
  Serial.begin(115200);
  servo.attach(2);   
  servo.write(angle);
}

void loop() {
  if (Serial.available()) {
    int target = Serial.parseInt();
    target = constrain(target, 0, 180);

    while (angle != target) {
      angle += (target > angle) ? 1 : -1;
      servo.write(angle);
      delay(8);   
    }
}

}
