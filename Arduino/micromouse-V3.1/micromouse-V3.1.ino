//Thư viện cho Flood Fill
#include <ArduinoQueue.h>
#include <PID_v1.h>

//Hiệu chỉnh các tham số--------------------------------------------------
//---------Tham số cho PID-------------------------------------------
const double Kp1=20, Ki1=0.5, Kd1=0.06;
const double Kp2=20, Ki2=0.25, Kd2=0.01;
const double turnKp1=20, turnKi1=0, turnKd1=0.06;
const double turnKp2=20, turnKi2=0, turnKd2=0.06;
const double KpC=8, KiC=0.5, KdC=0.06;

//---------Độ dài 1 bước, số encoder tại khúc cua--------------------
const uint16_t oneStep = 764; //bánh xe có đường kính là 34mm => chu vi = 34pi mà 1 ô có chiều dài = 18cm, 1 vòng encoder có 20 rãnh => số rãnh cho 1 step
const int oneTurn = 753; //khoảng cách giữa 2 bánh xe là 9cm => chu vi  = 9pi, muốn quay 90 dộ thì cần quay tròn 1 khoảng = 1/4 chu vi
const uint16_t backward = -100;
const uint16_t forward = 156;
const uint16_t lack1 = 0; //PID bị thiếu 2 xung ở motor 1 nên bù thêm 2 xung vào Setpoint1
const uint16_t lack2 = 0; //PID bị thiếu 1 xung ở motor 2 nên bù thêm 1 xung vào Setpoint2
const int turnLack = 9;
//---------Ngưỡng của các cảm biến khi có tường ở phía trước---------
const uint16_t threshold1 = 100; //led 1 nhạy nhất nên threshold cao nhất
const uint16_t threshold2 = 100;  //led 2 kém nhạy nhất
const uint16_t threshold3 = 100;  //led 3 nhạy ở giữa
//---------Tuỳ chỉnh kích thước mê cung------------------------------
const uint8_t maze_size = 16; //16x16
//Hết phần hiệu chỉnh-----------------------------------------------------

unsigned long Start;

//Define Variables we'll be connecting to
double Setpoint1;
double Setpoint2;
double SetpointC;
double Input1, Output1, Input2, Output2, InputC, OutputC;

PID motor1(&Input1, &Output1, &Setpoint1, Kp1, Ki1, Kd1, DIRECT);
PID motor2(&Input2, &Output2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);
PID Calibrate(&InputC, &OutputC, &SetpointC, KpC, KiC, KdC, DIRECT);



uint8_t coordinates[16][16][4];
uint8_t floodfill[16][16];
uint8_t x,y; //biến lưu toạ độ hiện tại của xe
uint8_t o = 1;
uint8_t a,b; //biến để chạy floodfill
bool reach_goal;


struct point {
  uint8_t x;
  uint8_t y ;
};


//PINOUT cho Motor
int IN3 = 0;  // Motor trái
int IN4 = 2;
int IN1 = 16;  // Motor phải
int IN2 = 4;
//PINOUT cho Encoder
#define encoderPin1A 17
#define encoderPin1B 5
#define encoderPin2A 22
#define encoderPin2B 23
//---------------------

int encoder1;
int encoder2;


//PINOUT cho cảm IR
//Sensor 1 (bên phải)
#define irE1 8
#define irR1 36
//Sensor 2 (phía trước)
#define irE2 11
#define irR2 26
//Sensor 3 (bên trái)
#define irE3 7
#define irR3 14

#define irControl 13

//khai báo các hàm
  //Di chuyển
void count1();
void count2();
void tricklor();
void moveForward();
void turnLeft();
void turnRight();
  //Xác định tường
bool wallLeft();
bool wallFront();
bool wallRigth();
  //Flood Fill (giải mê cung)
void wall(uint8_t x, uint8_t y);
void Oxy();
void goal_detemine();
void Flood_Fill();
void change_direction (uint8_t Direction);
void Move();


void setup() {
  Serial.begin(115200);
  // delay(5000);

  //các chân cho motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(encoderPin1B, INPUT);
  pinMode(encoderPin2B, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPin2A), count2, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPin1A), count1, RISING);
  //initialize the variables we're linked to
  Input1 = encoder1;
  Input2 = encoder2;
  InputC = analogRead(irR1) - analogRead(irR3);

  //PID-----------------------------
  motor1.SetMode(AUTOMATIC);
  motor1.SetSampleTime(1);
  motor1.SetOutputLimits(-127, 127);
  //turn the PID motor2 on
  motor2.SetMode(AUTOMATIC);
  motor2.SetSampleTime(1);
  motor2.SetOutputLimits(-107, 107);

  Calibrate.SetMode(AUTOMATIC);
  Calibrate.SetSampleTime(1);
  Calibrate.SetOutputLimits(-127, 127);
  //--------------------------------
  

  pinMode(irControl, OUTPUT);
  digitalWrite(irControl, 0);

  //các chân cho IR
  // pinMode(irE1, OUTPUT);
  // pinMode(irE2, OUTPUT);
  // pinMode(irE3, OUTPUT);  
  // digitalWrite(irE1, 0);
  // digitalWrite(irE2, 0);
  // digitalWrite(irE3, 0);


  delay(1000);

  //Mặc định sẽ có tường ở sau lưng của xe nên set = 1;
  coordinates[0][0][3] = 1;


  //giải mê cung


  while(!reach_goal) {
    
    wall(x,y);
    //log("wall ok");
    //show_wall(x,y);
    //log("show_wall ok");
    Flood_Fill();
    //log("Flood_Fill ok");
    //log((floodfill[x][y]));
    Move();
    //log("move ok");
    Oxy();
    //log("Oxy ok");
    goal_detemine();

    Serial.print("wallLeft:");
    Serial.print(wallLeft());
    Serial.print(",");
    Serial.print("wallFront:");
    Serial.print(wallFront());
    Serial.print(",");
    Serial.print("wallRight:");
    Serial.println(wallRight());
  }
  
  Flood_Fill();

}

void loop() {


  Serial.print(analogRead(irR1));
  Serial.print("   ");
  Serial.print(analogRead(irR2));
  Serial.print("   ");
  Serial.println(analogRead(irR3));


  //hiệu của 2 cảm biến không quá 500 thì calibrate
  

  
}
