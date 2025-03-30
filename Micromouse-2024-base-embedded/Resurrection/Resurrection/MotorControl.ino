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
    analogWrite(IN2, constrain(motorL, -250, 250)); //Motor trái hạn chế ở mức 250 để cân bằng với motor phải
  } else if (motorL < 0) {
    analogWrite(IN1, constrain(-motorL, -250, 250));
    analogWrite(IN2, 0);
  } else {
    analogWrite(IN1, 255);
    analogWrite(IN2, 255);
  }

  if (motorR > 0) {
    analogWrite(IN3, 0);
    analogWrite(IN4, constrain(motorR, -255, 255));
  } else if (motorR < 0) {
    analogWrite(IN3, constrain(-motorR, 255, 255));
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

  lastErr = error;
  lastTime = now;
  return (int)Output;
}

void turnRight() {
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = 460;
  int Setpoint2 = -460;


  float kpL = 3, kiL = 0, kdL = 0.1;
  float kpR = 3, kiR = 0, kdR = 0.1;
  int leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
  int rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR); //Motor phải quay chậm hơn nên thêm độ chênh vào setpoint để bù phần chênh
  while(encoderL < Setpoint1 || encoderR > Setpoint2) {
    leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
    rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR);
    motorControl(leftMotorControl, rightMotorControl);
    Serial.print(encoderL);
    Serial.print(",");
    Serial.print(leftMotorControl);
    Serial.print(",");
    Serial.print(encoderR);
    Serial.print(",");
    Serial.println(rightMotorControl);
  }

  motorControl(0,0);
}

void turnLeft() {
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = -460;
  int Setpoint2 = 460;


  float kpL = 3, kiL = 0, kdL = 0.1;
  float kpR = 3, kiR = 0, kdR = 0.1;
  int leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
  int rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR); //Motor phải quay chậm hơn nên thêm độ chênh vào setpoint để bù phần chênh
  while(encoderL > Setpoint1 || encoderR < Setpoint2) {
    leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
    rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR);
    motorControl(leftMotorControl, rightMotorControl);
    Serial.print(encoderL);
    Serial.print(",");
    Serial.print(leftMotorControl);
    Serial.print(",");
    Serial.print(encoderR);
    Serial.print(",");
    Serial.println(rightMotorControl);
  }

  motorControl(0,0);
}

void turnAround() {
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = 985;
  int Setpoint2 = -985;


  float kpL = 3, kiL = 0, kdL = 0.1;
  float kpR = 3, kiR = 0, kdR = 0.1;
  int leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
  int rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR); //Motor phải quay chậm hơn nên thêm độ chênh vào setpoint để bù phần chênh
  while(encoderL < Setpoint1 || encoderR > Setpoint2) {
    leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
    rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR);
    motorControl(leftMotorControl, rightMotorControl);
    Serial.print(encoderL);
    Serial.print(",");
    Serial.print(leftMotorControl);
    Serial.print(",");
    Serial.print(encoderR);
    Serial.print(",");
    Serial.println(rightMotorControl);
  }

  motorControl(0,0);
}

void moveForwardMPU(int oneStep) { //Số xung
  encoderL = 0;
  encoderR = 0;
  mpu6050.resetAngleZ(0);  //Đưa góc Z về lại 0
  const float targetAngle = 0;
  const float tolerance = 0.5;
  const float kp = 10;
  const float ki = 0;
  const float kd = 1;
  int turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);

  unsigned int start = millis();
  while (encoderL < oneStep || encoderR < oneStep && millis()-start < 2000) {
    turnCalibrate = simplePID(targetAngle, mpu6050.getAngleZ(), kp, ki, kd);
    if(encoderL < oneStep && encoderR < oneStep) {
      motorControl((minSpeed - constrain(turnCalibrate, 0, maxSpeed)), (minSpeed + constrain(turnCalibrate, 0, maxSpeed)));
    }else if(encoderL >= oneStep && encoderR < oneStep) {
      motorControl(0, (minSpeed + constrain(turnCalibrate, 0, maxSpeed)));
    }else if(encoderL < oneStep && encoderR >= oneStep) {
      motorControl((minSpeed - constrain(turnCalibrate, 0, maxSpeed)), 0);
    }
    Serial.print(encoderL);
    Serial.print(",");
    Serial.println(encoderR);
    // Serial.print(mpu6050.getAngleZ());
    // Serial.print(",");
    // Serial.println(turnCalibrate);
    strip.fill(strip.Color(0, 0, 128, 0));
    strip.show();
  }
  // motorControl(0, 0);
  strip.fill(strip.Color(0, 128, 0, 0));  //Chạy xong thì sáng lục
  strip.show();
}

void moveForwardEncoder() {
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = 1151;
  int Setpoint2 = 1151;


  float kpL = 3, kiL = 0, kdL = 0.01;
  float kpR = 3, kiR = 0, kdR = 0.01;
  int leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
  int rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR); //Motor phải quay chậm hơn nên thêm độ chênh vào setpoint để bù phần chênh
  while(encoderL < Setpoint1 || encoderR < Setpoint2) {
    leftMotorControl = simplePID(Setpoint1, encoderL, kpL, kiL, kdL);
    rightMotorControl = simplePID(Setpoint2, encoderR, kpR, kiR, kdR);
    motorControl(leftMotorControl, rightMotorControl);
    Serial.print(encoderL);
    Serial.print(",");
    // Serial.print(leftMotorControl);
    // Serial.print(",");
    Serial.println(encoderR);
    // Serial.print(",");
    // Serial.println(rightMotorControl);
  }

  motorControl(0,0);
}

void moveForward() {
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  
  encoderL = 0;
  encoderR = 0;
  int Setpoint1 = 1196 - errorTurn;
  int Setpoint2 = 1196 - errorTurn;

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
      moveForwardMPU((Setpoint1 + Setpoint2)/2 - (encoderL + encoderR)/2);
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

    errorTurn = 0;
  }


  // motorControl(0,0);
  strip.fill(strip.Color(0,0,0,0));
  strip.show();
  // delay(1000);

}