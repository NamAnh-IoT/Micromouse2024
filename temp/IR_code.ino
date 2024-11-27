// Chieu tu trai sang phai
int in1 = 14;
int in2 = 27;
int in3 = 26;
int in4 = 34;
int in5 = 39;
int in6 = 36;

// 27 26 34, 39, 36

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(9600);
  
  // Đặt các chân cần đọc giá trị thành INPUT
  pinMode(in1, INPUT);
  pinMode(in2, INPUT);
  pinMode(in3, INPUT);
  pinMode(in4, INPUT);
  pinMode(in5, INPUT);
  pinMode(in6, INPUT);
}

void loop() {
  // Đọc giá trị analog từ các chân
  int value1 = analogRead(in1);
  int value2 = analogRead(in2);
  int value3 = analogRead(in3);
  int value4 = analogRead(in4);
  int value5 = analogRead(in5);
  int value6 = analogRead(in6);

  // In giá trị của từng chân ra Serial Monitor
  Serial.print("Value at in1: ");
  Serial.println(value1);
  
  Serial.print("Value at in2: ");
  Serial.println(value2);
  
  Serial.print("Value at in3: ");
  Serial.println(value3);
  
  Serial.print("Value at in4: ");
  Serial.println(value4);
  
  Serial.print("Value at in5: ");
  Serial.println(value5);
  
  Serial.print("Value at in6: ");
  Serial.println(value6);
  
  // Thêm một độ trễ giữa các lần đọc
  delay(1000);  // Delay 1 giây
}
