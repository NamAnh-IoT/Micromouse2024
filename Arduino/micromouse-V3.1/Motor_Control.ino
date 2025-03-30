void count1() {
  int stateA = digitalRead(encoderPin1A);
  int stateB = digitalRead(encoderPin1B);
  if (stateA == HIGH) {
    if (stateB == LOW) {
      encoder1++; // Clockwise
    } else {
      encoder1--; // Counter-clockwise
    }
  } else {
    if (stateB == HIGH) {
      encoder1++; // Clockwise
    } else {
      encoder1--; // Counter-clockwise
    }
  }
  
  //Serial.print(F("Encoder1: ");
  //Serial.print(Fln(v1);
}
void count2() {
  int stateA = digitalRead(encoderPin2A);
  int stateB = digitalRead(encoderPin2B);
  if (stateA == HIGH) {
    if (stateB == LOW) {
      encoder2++; // Clockwise
    } else {
      encoder2--; // Counter-clockwise
    }
  } else {
    if (stateB == HIGH) {
      encoder2++; // Clockwise
    } else {
      encoder2--; // Counter-clockwise
    }
  }
  
  //Serial.print(F("Encoder2: ");
  //Serial.print(Fln(encoder2);
}

void motorControl() {
  //Input1 = encoder1;
  //Input2 = encoder2;
  //motor1.Compute();
  //motor2.Compute();

  
  if(Output1 > 0) {
    analogWrite(IN2, Output1);
    analogWrite(IN1, 0);
  }else if(Output1 == 0) {
    analogWrite(IN1, 255);
    analogWrite(IN2, 255);
  }else if(Output1 < 0) {
    analogWrite(IN2, 0);
    analogWrite(IN1, abs(Output1));
  }
  
  if(Output2 > 0) {
    analogWrite(IN4, Output2);
    analogWrite(IN3, 0);
  }else if(Output2 == 0) {
    analogWrite(IN3, 255);
    analogWrite(IN4, 255);
  }else if(Output2 < 0) {
    analogWrite(IN4, 0);
    analogWrite(IN3, abs(Output2));
  }

  Serial.println(F("nothing"));
  /*
  Serial.print(F("encoder1:"));
  Serial.print(encoder1);
  Serial.print(F(","));
  Serial.print(F("encoder2:"));
  Serial.print(encoder2);
  Serial.print(F(","));
  Serial.print(F("output1:"));
  Serial.print(Output1);
  Serial.print(F(","));
  Serial.print(F("output2:"));
  Serial.print(Output2);
  Serial.print(F(","));
  Serial.print(F("Setpoint11:"));
  Serial.print(Setpoint1);
  Serial.print(F(","));
  Serial.print(F("Setpoint2:"));
  Serial.println(Setpoint2);
  */
}

void moveForward() {

  encoder1 = 0;
  encoder2 = 0;
  //int a = 625; // Giá trị gốc của Setpoint1 
  Setpoint1 = oneStep;
  Setpoint2 = oneStep;

  motor1.SetTunings(Kp1, Ki1, Kd1);
  motor2.SetTunings(Kp2, Ki2, Kd2);

  Start = millis();
  while (millis() - Start < 4000) {
    if((encoder1 < Setpoint1 || encoder1 > Setpoint1) || (encoder2 < Setpoint2 || encoder2 > Setpoint2)) {
      Input1 = encoder1;
      Input2 = encoder2;
      
      motor1.Compute();
      motor2.Compute();

      if(abs(analogRead(irR1) - analogRead(irR3)) < 800 && abs(analogRead(irR1) - analogRead(irR3)) > 200 && analogRead(irR1) < 850 && analogRead(irR3) < 850) {
        InputC = analogRead(irR1) - analogRead(irR3);
        Calibrate.Compute();

        Output1 = constrain(Output1 + OutputC, 40, 255);
        // Output2 = constrain(Output2 - OutputC, 40, 174);
        Output2 = constrain(Output2 - OutputC, 40, 255);
        
      }
      motorControl();
    }
    delay(10);
  }
  
  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
  delay(200); 

}

void tricklor() {
  Start = millis();
  while(millis() - Start < 1750) {
    analogWrite(IN1, 127);
    analogWrite(IN2, 0);
    analogWrite(IN3, 107);
    analogWrite(IN4, 0);      
  }
  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
  delay(200);
  encoder1 = 0;
  encoder2 = 0;
  Start = millis();
  while(millis() - Start < 1000) {
    if(encoder1 < forward || encoder2 < forward) {
      analogWrite(IN1, 0);
      analogWrite(IN2, 127);
      analogWrite(IN3, 0);
      analogWrite(IN4, 107); 
    }else{
      Start = millis() - 1500;
    }
  }
  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
  delay(200);
}

void turnRight() {

  encoder1 = 0;
  encoder2 = 0;
  Setpoint1 = -oneTurn;
  Setpoint2 = oneTurn;

  motor1.SetTunings(turnKp1, turnKi1, turnKd1);
  motor2.SetTunings(turnKp2, turnKi2, turnKd2);

  bool turnBack = wallLeft();

  Start = millis();
  while (millis() - Start < 1500) {
    if ((encoder1 < Setpoint1 || encoder1 > Setpoint1) || (encoder2 < Setpoint2 || encoder2 > Setpoint2)) {
      Input1 = encoder1;
      Input2 = encoder2;
      motor1.Compute();
      motor2.Compute();
      motorControl();
    }
  }

  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
  delay(200);

  if(turnBack == 1) {
    tricklor();
  }
}

void turnLeft() {

  encoder1 = 0;
  encoder2 = 0;
  Setpoint1 = oneTurn;
  Setpoint2 = -oneTurn;

  motor1.SetTunings(turnKp1, turnKi1, turnKd1);
  motor2.SetTunings(turnKp2, turnKi2, turnKd2);

  bool turnBack = wallRight();

  Start = millis();
  while (millis() - Start < 1500) {
    if ((encoder1 < Setpoint1 || encoder1 > Setpoint1) || (encoder2 < Setpoint2 || encoder2 > Setpoint2)) {
      Input1 = encoder1;
      Input2 = encoder2;
      motor1.Compute();
      motor2.Compute();    
      motorControl();
    }
  }

  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
  delay(200);

  if(turnBack == 1) {
    tricklor();
  }

}
