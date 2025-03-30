//MPU quay phải là âm quay trái là dương

//Thư viện cho Flood Fill
#include <ArduinoQueue.h>
//MPU
#include <MPU6050_tockn.h>
#include <Wire.h>
//Led trên board
#include <Adafruit_NeoPixel.h>

//LED
Adafruit_NeoPixel strip(1, 23, NEO_GRBW + NEO_KHZ800); //số led, chân kết nối
//MPU
MPU6050 mpu6050(Wire);


#define usrButton 24

//Bên trái
#define IN1 7
#define IN2 6
//Bên phải
#define IN3 9
#define IN4 8

#define IRR1 28  // trai duoi
#define IRR2 27  // trai tren
#define IRR3 26  // phai tren
#define IRR4 29  // phai duoi

#define IRT1 18 //trên trái
#define IRT2 21 //phải
#define IRT3 20 //trái
#define IRT4 19 //trên phải

#define ENCODER_LEFT_A 12
#define ENCODER_LEFT_B 13
#define ENCODER_RIGHT_A 10
#define ENCODER_RIGHT_B 11

//Biến đêm encoder
int encoderL = 0;
int encoderR = 0;

//Biến giới hạn tốc độ
const int maxSpeed = 255; //max PWM value written to motor speed pin. It is typically 255.
const int minSpeed = 150; //min PWM value at which motor moves

// float elapsedTime, currentTime, previousTime;
double lastErr;
double lastTime;
double errSum;

//Biến cho thuật toán
const uint8_t maze_size = 16; //16x16
uint8_t coordinates[maze_size][maze_size][4];
uint8_t floodfill[maze_size][maze_size];
uint8_t x,y; //biến lưu toạ độ hiện tại của xe
uint8_t o = 1;
uint8_t a,b; //biến để chạy floodfill
bool reach_goal;

struct point {
  uint8_t x;
  uint8_t y ;
};

int errorTurn; //Do 2 bánh xe không ở giữa xe (đầu xe thò ra nhiều) nên sau khi quay đầu sẽ thò ra khỏi ô nên cần trừ đi phần thừa vào bước tiến tiếp theo


//Hàm điều khiển motor
void encoderLeftCount();
void encoderRightCount();
void motorControl(int motorL, int motorR);
int simplePID(float Setpoint, float Input, float kp, float ki, float kd);
void turnLeft();
void turnRight();
void moveForward();
void moveForwardMPU(int oneStep, bool direction);

//Xác định tường
bool wallLeft();
bool wallFront();
bool wallRight();

//Flood Fill (giải mê cung)
void wall(uint8_t x, uint8_t y);
void Oxy();
void goal_detemine();
void Flood_Fill();
void change_direction (uint8_t Direction);
void Move();



void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.fill(strip.Color(128,0,0,0));
  strip.show();            // Turn OFF all pixels ASAP
  //Sáng đỏ khi MPU đo offset

  Wire.begin();
  mpu6050.begin();
  // mpu6050.calcGyroOffsets(true); //Hàm tự đo offset (hiển thị console, delay trước, delay sau) mặc định delay sau = 3s
  mpu6050.setGyroOffsets(-0.64, 1.85, -0.78);

  pinMode(usrButton, INPUT_PULLUP);

  delay(20);
  //L298 - motor driver
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  //Encoder
  pinMode(ENCODER_LEFT_A, INPUT);
  pinMode(ENCODER_LEFT_B, INPUT);
  pinMode(ENCODER_RIGHT_A, INPUT);
  pinMode(ENCODER_RIGHT_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_A), encoderLeftCount, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_A), encoderRightCount, CHANGE);
  
  //IR Phát
  pinMode(IRT1,OUTPUT);
  pinMode(IRT2,OUTPUT); 
  pinMode(IRT3,OUTPUT);
  pinMode(IRT4,OUTPUT);

  digitalWrite(IRT1, 1);
  digitalWrite(IRT2, 1);
  digitalWrite(IRT3, 1);
  digitalWrite(IRT4, 1);


  int last = millis();
  bool change = 0;
  while(digitalRead(usrButton) == 1) {
    // targetIrLeft = analogRead(IRR1);
    // targetIrRight = analogRead(IRR4);

    if(millis() - last >= 250) {
      change = !change;
      last = millis();
    }
    if(change) {
      strip.fill(strip.Color(128,0,0,0));
      strip.show();
    }else{
      strip.fill(strip.Color(0,0,0,0));
      strip.show();
    }
  }
  strip.fill(strip.Color(128,128,128,0));
  strip.show();
  mpu6050.update();
  delay(1000);


  //Mặc định sẽ có tường ở sau lưng của xe nên set = 1;
  coordinates[0][0][3] = 1;

  //FloodFill lần đầu
  while(!reach_goal) {
    wall(x,y);
    //show_wall(x,y);
    Flood_Fill();
    Move();
    Oxy();
    goal_detemine();
  }

  while(true) {
    motorControl(0, 0);
    while(digitalRead(usrButton) == 1) {
      // targetIrLeft = analogRead(IRR1);
      // targetIrRight = analogRead(IRR4);

      if(millis() - last >= 250) {
        change = !change;
        last = millis();
      }
      if(change) {
        strip.fill(strip.Color(128,0,0,0));
        strip.show();
      }else{
        strip.fill(strip.Color(0,0,0,0));
        strip.show();
      }
    }
    strip.fill(strip.Color(128,128,128,0));
    strip.show();
    mpu6050.update();
    delay(1000);

    
    //FloodFill lần 2
    x = 0;
    y= 0;
    o = 1;
    reach_goal = 0;
    while(!reach_goal) {
      wall(x,y);
      //show_wall(x,y);
      Flood_Fill();
      Move();
      Oxy();
      goal_detemine();
    }

  }  
 
  
}

void loop() {

  
  // turnRight();
  // turnAround();
  // delay(2000);

  // Serial.print(analogRead(IRR1));
  // Serial.print(", ");
  // Serial.print(analogRead(IRR2));
  // Serial.print(", ");
  // Serial.print(analogRead(IRR3));
  // Serial.print(", ");
  // Serial.println(analogRead(IRR4));
  // delay(10);

  // Serial.print(encoderL);
  // Serial.print(",");
  // Serial.println(encoderR);


}
