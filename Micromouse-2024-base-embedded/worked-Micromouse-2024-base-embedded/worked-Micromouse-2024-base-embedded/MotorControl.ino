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
  const float targetAngle = 87;
  const float tolerance = 0.5;
  const float kp = 5;
  const float ki = 0;
  const float kd = 0;
  int turnCalibrate = simplePID(targetAngle, abs(mpu6050.getAngleZ()), kp, ki, kd);
  
  unsigned int start = millis();
  while (!(targetAngle + tolerance > abs(mpu6050.getAngleZ()) && abs(mpu6050.getAngleZ()) > targetAngle - tolerance) && millis()-start < 2000) {
    mpu6050.update();
    turnCalibrate = simplePID(targetAngle, abs(mpu6050.getAngleZ()), kp, ki, kd);
    motorControl(constrain(turnCalibrate, -255, 255), -constrain(turnCalibrate, -255, 255));
    Serial.print(mpu6050.getAngleZ());
    Serial.print(",");
    Serial.println(turnCalibrate);
    // mpu6050.resetAngleZ(0); //Đưa góc Z về lại 0
    strip.fill(strip.Color(0, 0, 128, 0));
    strip.show();
    delay(5);
  }
  motorControl(0, 0);
  if(tricklor) {
    // encoderL = 0; //Reset encoder
    // encoderR = 0;
    // moveForwardMPU(400, -1);
    unsigned int start = millis();
    while(millis() - start < 500) {
      motorControl(-minSpeed,-minSpeed);
    }
    delay(100);
    encoderL = 0; //Reset encoder
    encoderR = 0;
    moveForwardMPU(200,1);
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
  const float targetAngle = 87;  //PID tính số âm không ổn lắm nên thêm trị truyệt đối
  const float tolerance = 0.5;
  const float kp = 5;
  const float ki = 0;
  const float kd = 0;
  int turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
  
  unsigned int start = millis();
  while (!(targetAngle + tolerance > mpu6050.getAngleZ() && mpu6050.getAngleZ() > targetAngle - tolerance) && millis()-start < 2000) {
    mpu6050.update();
    turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
    motorControl(-constrain(turnCalibrate, -255, 255), constrain(turnCalibrate, -255, 255));
    Serial.print(mpu6050.getAngleZ());
    Serial.print(",");
    Serial.println(turnCalibrate);
    // mpu6050.resetAngleZ(0); //Đưa góc Z về lại 0
    strip.fill(strip.Color(0, 0, 128, 0));
    strip.show();
    delay(5);
  }
  motorControl(0, 0);
  //Lùi đít khi quay
  if(tricklor) {
    // encoderL = 0; //Reset encoder
    // encoderR = 0;
    // moveForwardMPU(400, -1);
    unsigned int start = millis();
    while(millis() - start < 500) {
      motorControl(-minSpeed,-minSpeed);
    }
    delay(100);
    encoderL = 0; //Reset encoder
    encoderR = 0;
    moveForwardMPU(200,1);
  }

  strip.fill(strip.Color(0, 128, 0, 0));  //Chạy xong thì sáng lục
  strip.show();
  // delay(1000);
}

void moveForwardMPU(int oneStep, int direction) { //Số xung, hướng (1 tiến, -1 là lùi)
  // mpu6050.update();
  mpu6050.resetAngleZ(0);  //Đưa góc Z về lại 0
  const float targetAngle = 0;
  const float tolerance = 0.5;
  const float kp = 10;
  const float ki = 0;
  const float kd = 1;
  int turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);

  // encoderL = 0; //Reset encoder
  // encoderR = 0;
  // int oneStep = 1196;  //tính bằng độ dài 1 bước chia cho chu vi bánh xe nhân số xung/vòng
  unsigned int start = millis();
  while (encoderL < oneStep && millis()-start < 2000) {
    mpu6050.update();
    turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
    motorControl(direction * (minSpeed - constrain(turnCalibrate, 0, maxSpeed)), direction * (minSpeed + constrain(turnCalibrate, 0, maxSpeed)));
    // Serial.print(encoderL);
    // Serial.print(",");
    // Serial.print(mpu6050.getAngleZ());
    // Serial.print(",");
    // Serial.println(turnCalibrate);
    strip.fill(strip.Color(0, 0, 128, 0));
    strip.show();
  }
  motorControl(0, 0);
  strip.fill(strip.Color(0, 128, 0, 0));  //Chạy xong thì sáng lục
  strip.show();
}

void moveForward() {
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = 1196;
  int Setpoint2 = 1196;

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

  while (((encoderL + encoderR)/2 < (Setpoint1 + Setpoint2)/2) && (millis() - Start < 2000)) {
    // Can 2 tuong
    if(wallLeft() && wallRight()) {
      irError = analogRead(IRR1) - analogRead(IRR4) + offset; // Sai số căn giữa tường
      Serial.print("2 tuong   ");
      strip.fill(strip.Color(128,128,0,0));
      strip.show();
    // Can tuong trai
    }else if(wallLeft() && !wallRight()){
      irError = analogRead(IRR1) - targetIrLeft; 
      Serial.print("tuong trai  ");
      strip.fill(strip.Color(128,0,0,0));
      strip.show();
    // Can tuong phai
    }else if(!wallLeft() && wallRight()){
      irError = targetIrRight - analogRead(IRR4);
      Serial.print("tuong phai   ");
      strip.fill(strip.Color(0,128,0,0));
      strip.show();
    }else{
      Serial.print("0 tuong");
      moveForwardMPU(1196, 1);
      break;
    }
    Serial.print(irError);
    Serial.print(",");
    Serial.print(analogRead(IRR1));
    Serial.print(",");
    Serial.print(analogRead(IRR4));
    Serial.print(",");
    Serial.println(calibrate);

    // filteredValue = alpha * irError + (1 - alpha) * filteredValue;
    // calibrate = simplePID(0, filteredValue, 0.2,0,0);
    calibrate = simplePID(0, irError, 0.2,0,0.05);

    motorControl(minSpeed - constrain(calibrate, -minSpeed+50, minSpeed-50), minSpeed + constrain(calibrate, -minSpeed+50, minSpeed-50));
    // motorControl(maxSpeed - constrain(calibrate, 30, maxSpeed-30), maxSpeed + constrain(calibrate, 30, maxSpeed-30));
    // motorControl(constrain(minSpeed - calibrate, 30, maxSpeed), constrain(minSpeed + calibrate, 30, maxSpeed));
  }


  motorControl(0,0);
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  // delay(1000);

}

void frontAlign() {
  motorControl(0,0);
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  const int tolerance = 50; //Dung sai (độ lệch tối đa của 2 ir đằng trước)
  const int targetL = 1800;
  const int targetR = 2800;
  float turnCalibrate;
  float moveCalibrate;
  unsigned int start = millis();
  if(wallFront()) {
    // while(abs(analogRead(IRR2) - analogRead(IRR3)) > tolerance || !((target-tolerance < analogRead(IRR2)) && (analogRead(IRR2) < target+tolerance))) {
    while((!((targetL-tolerance < analogRead(IRR2)) && (analogRead(IRR2) < targetL+tolerance)) || !((targetR-tolerance < analogRead(IRR3)) && (analogRead(IRR3) < targetR+tolerance))) && (millis()-start < 2000)) {
      strip.fill(strip.Color(128,0,0,0));
      strip.show();
      // turnCalibrate = simplePID(0, analogRead(IRR2) - analogRead(IRR3), 0.25,0.05,0);
      // moveCalibrate = simplePID(target, analogRead(IRR2), 0.25,0.05,0);

      // motorControl(constrain(moveCalibrate, -maxSpeed, maxSpeed) + constrain(turnCalibrate, -maxSpeed, maxSpeed), constrain(moveCalibrate, -maxSpeed, maxSpeed) - constrain(turnCalibrate, -maxSpeed, maxSpeed));
      
      int leftCalibrate = simplePID(targetL, analogRead(IRR2), 0.4,0.02,0);
      int rightCalibrate = simplePID(targetR, analogRead(IRR3), 0.4,0.02,0);
      motorControl(constrain(leftCalibrate, -maxSpeed, maxSpeed), constrain(rightCalibrate, -maxSpeed, maxSpeed));
      
      Serial.print(leftCalibrate);
      Serial.print(", ");
      Serial.print(analogRead(IRR2));
      Serial.print(", ");
      Serial.print(rightCalibrate);
      Serial.print(", ");
      Serial.println(analogRead(IRR3));
    }
    // while((target-tolerance < analogRead(IRR2)) && (analogRead(IRR2) < target+tolerance)) {
    //   strip.fill(strip.Color(0,0,128,0));
    //   strip.show();
    //   calibrate = simplePID(0, analogRead(IRR2), 0.2,0,0);
    //   motorControl(-constrain(calibrate, -minSpeed, minSpeed), -constrain(calibrate, -minSpeed, maxSpeed));

    //   Serial.print(analogRead(IRR2) - analogRead(IRR3));
    //   Serial.print(", ");
    //   Serial.print(analogRead(IRR2));
    //   Serial.print(", ");
    //   Serial.println(analogRead(IRR3));
    // }
  }
  Serial.print(analogRead(IRR2) - analogRead(IRR3));
  Serial.print(", ");
  Serial.print(analogRead(IRR2));
  Serial.print(", ");
  Serial.println(analogRead(IRR3));

  Serial.println("done");
  motorControl(0,0);
  strip.fill(strip.Color(0,128,0,0));
  strip.show();
}