void encoderLeftCount() {  //Do 2 con motor lắp ngược nên 1 con phải đọc ngược lại
  if (digitalRead(ENCODER_LEFT_A) == digitalRead(ENCODER_LEFT_B)) {
    encoderL--;
  } else {
    encoderL++;
  }
}


void encoderRightCount() {
  if (digitalRead(ENCODER_RIGHT_A) == digitalRead(ENCODER_RIGHT_B)) {
    encoderR++;
  } else {
    encoderR--;
  }
}

void motorControl(int motorL, int motorR) {
  if (motorL > 0) {
    analogWrite(IN1, 0);
    analogWrite(IN2, motorL);
  } else if (motorL < 0) {
    analogWrite(IN1, -motorL);
    analogWrite(IN2, 0);
  } else {
    analogWrite(IN1, 255);
    analogWrite(IN2, 255);
  }

  if (motorR > 0) {
    analogWrite(IN3, 0);
    analogWrite(IN4, motorR);
  } else if (motorR < 0) {
    analogWrite(IN3, -motorR);
    analogWrite(IN4, 0);
  } else {
    analogWrite(IN3, 255);
    analogWrite(IN4, 255);
  }
}

int simplePID(float Setpoint, float Input, float kp, float ki, float kd) {
  /*How long since we last calculated*/
  unsigned long now = micros();
  double timeChange = (double)(now - lastTime) * 0.000001;

  if(timeChange == 0) {
    Serial.println("ERROR: timeChange = 0");
  }

  /*Compute all the working error variables*/
  double error = Setpoint - Input;
  errSum += (error * timeChange);
  double dErr = (error - lastErr) / timeChange;

  /*Compute PID Output*/
  float Output = kp * error + ki * errSum + kd * dErr;

  // Serial.print(timeChange);
  // Serial.print(",");
  // Serial.print(error);
  // Serial.print(",");
  // Serial.print(errSum);
  // Serial.print(",");
  // Serial.println(dErr);
  // Serial.print(",");

  /*Remember some variables for next time*/
  lastErr = error;
  lastTime = now;
  return (int)Output;
}

void turnRight() {
  // frontAlign();

  bool tricklor = 0;
  if(wallLeft()) {
    tricklor = 1;
  }

  mpu6050.resetAngleZ(0);  //Đưa góc Z về lại 0
  mpu6050.update();
  const float targetAngle = 84;
  const float tolerance = 0.1;
  const float kp = 5;
  const float ki = 0;
  const float kd = 0;
  int turnCalibrate = simplePID(targetAngle, abs(mpu6050.getAngleZ()), kp, ki, kd);
  
  unsigned int start = millis();
  while(millis() - start < 1000) {
    mpu6050.update();
    if(!(targetAngle + tolerance > abs(mpu6050.getAngleZ()) && abs(mpu6050.getAngleZ()) > targetAngle - tolerance)) {
      turnCalibrate = simplePID(targetAngle, abs(mpu6050.getAngleZ()), kp, ki, kd);
      motorControl(constrain(turnCalibrate, -255, 255), -constrain(turnCalibrate, -255, 255));
      Serial.print(mpu6050.getAngleZ());
      Serial.print(",");
      Serial.println(turnCalibrate);
      // mpu6050.resetAngleZ(0); //Đưa góc Z về lại 0
      strip.fill(strip.Color(0, 0, 128, 0));
      strip.show();
      // delay(5);
    }
  }
  
  motorControl(0, 0);
  if(tricklor) {
    encoderL = 0; //Reset encoder
    encoderR = 0;
    while((encoderL + encoderR)/2 > -300) {
      motorControl(-minSpeed,-minSpeed);
    }

    delay(50);
    encoderL = 0; //Reset encoder
    encoderR = 0;
    moveForwardMPU(200,1);
  }else{

    encoderL = 0; //Reset encoder
    encoderR = 0;
    while((encoderL + encoderR)/2 > -100) {
      motorControl(-minSpeed,-minSpeed);
    }
    motorControl(0,0);
  }

  strip.fill(strip.Color(0, 128, 0, 0));  //Chạy xong thì sáng lục
  strip.show();
  // delay(1000);
}

void turnLeft() {
  // frontAlign();

  bool tricklor = 0;
  if(wallRight()) {
    tricklor = 1;
  }

  mpu6050.resetAngleZ(0);  //Đưa góc Z về lại 0
  mpu6050.update();
  const float targetAngle = 84;  //PID tính số âm không ổn lắm nên thêm trị truyệt đối
  const float tolerance = 0.1;
  const float kp = 5;
  const float ki = 0;
  const float kd = 0;
  int turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
  
  unsigned int start = millis();
  while(millis() - start < 1000) {
    mpu6050.update();
    if(!(targetAngle + tolerance > mpu6050.getAngleZ() && mpu6050.getAngleZ() > targetAngle - tolerance) && millis()-start < 2000) {
      turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
      motorControl(-constrain(turnCalibrate, -255, 255), constrain(turnCalibrate, -255, 255));
      Serial.print(mpu6050.getAngleZ());
      Serial.print(",");
      Serial.println(turnCalibrate);
      // mpu6050.resetAngleZ(0); //Đưa góc Z về lại 0
      strip.fill(strip.Color(0, 0, 128, 0));
      strip.show();
      // delay(5);
    }
  }
  motorControl(0, 0);

  //Lùi đít khi quay
  if(tricklor) {
    encoderL = 0; //Reset encoder
    encoderR = 0;
    while((encoderL + encoderR)/2 > -300) {
      motorControl(-minSpeed,-minSpeed);
    }

    delay(100);
    encoderL = 0; //Reset encoder
    encoderR = 0;
    moveForwardMPU(200,1);
  }else{
    encoderL = 0; //Reset encoder
    encoderR = 0;
    while((encoderL + encoderR)/2 > -100) {
      motorControl(-minSpeed,-minSpeed);
    }
    motorControl(0,0);
  }

  strip.fill(strip.Color(0, 128, 0, 0));  //Chạy xong thì sáng lục
  strip.show();
}

void moveForwardMPU(int oneStep, int direction) { //Số xung, hướng (1 tiến, -1 là lùi)
  mpu6050.resetAngleZ(0);  //Đưa góc Z về lại 0
  const float targetAngle = 0;
  const float tolerance = 0.5;
  const float kp = 10;
  const float ki = 0;
  const float kd = 1;
  int turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);

  encoderL = 0; //Reset encoder
  encoderR = 0;
  // int oneStep = 1196;  //tính bằng độ dài 1 bước chia cho chu vi bánh xe nhân số xung/vòng
  
  // strip.fill(strip.Color(0, 0, 128, 0));
  // strip.show();
  unsigned int start = millis();
  while ((encoderL < oneStep && millis()-start < 2000)) {
    mpu6050.update();
    turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
    motorControl(direction * (minSpeed - constrain(turnCalibrate, 0, maxSpeed)), direction * (minSpeed + constrain(turnCalibrate, 0, maxSpeed)));
    // Serial.print(encoderL);
    // Serial.print(",");
    // Serial.print(mpu6050.getAngleZ());
    // Serial.print(",");
    // Serial.println(turnCalibrate);
  }
  // motorControl(0, 0);
  strip.fill(strip.Color(0, 128, 0, 0));  //Chạy xong thì sáng lục
  strip.show();
}

void moveForward() {
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = 1200;
  int Setpoint2 = 1200;

  // const float targetIrLeft = 1535;
  // const float targetIrRight = 1535;
  const float targetIrLeft = 1535;
  const float targetIrRight = 1535;

  int calibrate;

  int Start = millis();
  int irError;
  int tolerance = 50; //Dung sai để ngừng căn tường
  float alpha = 0.2; //(từ 0 đến 1) càng lớn phản hồi càng nhanh, càng nhỏ độ mịn càng tốt
  float filteredValue; //Biến để lọc nhiễu LowPass Filter
  int offset = 800;

  bool startPoint = 0;
  bool endPoint = 0;

  if(wallLeft() && wallRight()) {
    startPoint = 1;
  }else{
    startPoint = 0;
  }

  endPoint = startPoint;

  // while (((encoderL + encoderR)/2 < (Setpoint1 + Setpoint2)/2) && (millis() - Start < 2000)) {

  while(true) {
    if(wallLeft() && wallRight()) {
      endPoint = 1;
    }else{
      endPoint = 0;
    }

    Serial.print(startPoint);
    Serial.print(",");
    Serial.println(endPoint);

    if(endPoint == !startPoint) {
      strip.fill(strip.Color(0,128,0,0));
      strip.show();
      if(endPoint == 0) {
        moveForwardMPU(500, 1);
      }else{
        moveForwardMPU(700, 1); //Độ chênh lệch so với hàm ở trên phụ thuộc chiều rộng của cột nhôm định hình
      }
      break;
    }
    motorControl(95,100);
    delay(5);
  }


  // motorControl(0,0);
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  // delay(1000);

}