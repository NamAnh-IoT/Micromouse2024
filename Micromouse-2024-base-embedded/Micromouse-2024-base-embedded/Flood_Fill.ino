void wall(uint8_t x, uint8_t y) {
  if(wallFront()){
    coordinates[x][y][o] = 1;
  }
  if(wallLeft()){
    coordinates[x][y][(o+3)%4] = 1;
  }
  if(wallRight()){
    coordinates[x][y][(o+1)%4] = 1;
  }
  
  
  //Do mỗi toạ độ đều lưu vị trí 4 hường tường nên sẽ bình chồng lên nhau, ví dụ ô 0,0 có tường bên phải thì ô 1,0 có tường ở bên trái
  if(coordinates[x][y][0] == 1 && x > 0 && coordinates[x-1][y][2] == 0) {
    coordinates[x-1][y][2] = 1;
  }
  if(coordinates[x][y][1] == 1 && y < maze_size-1 && coordinates[x][y+1][3] == 0) {
    coordinates[x][y+1][3] = 1;
  }
  if(coordinates[x][y][2] == 1 && x < maze_size-1 && coordinates[x+1][y][0] == 0) {
    coordinates[x+1][y][0] = 1;
  }
  if(coordinates[x][y][3] == 1 && y > 0 && coordinates[x][y-1][1] == 0) {
    coordinates[x][y-1][1] = 1;
  }

}


void Oxy() {
  switch (o) {
    case 0:
      x = x - 1;
      break;
    case 1:
      y = y + 1;
      break;
    case 2:
      x = x + 1;
      break;
    case 3:
      y = y - 1;
      break;
    default:
      break;
  }
}

/*
void show_wall(uint8_t x, uint8_t y) {
  if(coordinates[x][y][0] == 1) {
    setWall(x,y,'w');
  }
  if(coordinates[x][y][1] == 1) {
    setWall(x,y,'n');
  }
  if(coordinates[x][y][2] == 1) {
    setWall(x,y,'e');
  }
  if(coordinates[x][y][3] == 1) {
    setWall(x,y,'s');
  }
}
*/

void goal_detemine(int mode) {
  
  if((x == maze_size/2-1 || x == maze_size/2) && (y == maze_size/2-1 || y == maze_size/2)) {
    reach_goal = 1;
    coordinates[x][y][o] == 1;
    coordinates[x][y][(o+1) & 0x03] == 1;
    coordinates[x][y][(o-1) & 0x03] == 1;
  }

  if(mode == 1 && x == 0 && y == 0) {
    reach_goal = 1;
  }
    
}


void Flood_Fill(int mode) { //mode = 0: về đích | mode = 1: về điểm xuất phát
  ArduinoQueue<point> oxy(64); //tính nhẩm thì cần lưu được tối thiểu 8*4 = 32 nên để 64 cho chắc
  //Đưa hết các ô về blank (giá trị = 0)
  for(uint8_t m = 0; m < maze_size; m++) {
    for(uint8_t n = 0; n < maze_size; n++) {
      floodfill[m][n] = 0;
      //setText(m,n, "");
    }
  }
  

  switch (mode) {
    case 0:
        //Set các ô đích = 1 và cho vào hàng chờ
        for(uint8_t m = maze_size/2-1; m <= maze_size/2; m++) {
          for(uint8_t n = maze_size/2-1; n <=maze_size/2 ; n++) {
            floodfill[m][n] = 1;
            oxy.enqueue({m,n});
            //setText(m,n, String(floodfill[m][n]));
          }
        }
      break;
    case 1:
      floodfill[0][0] = 1;
      oxy.enqueue({0,0});
      break;
  }
  
  
  while(!oxy.isEmpty()) {
    a = oxy.getHead().x;
    b = oxy.getHead().y;
    oxy.dequeue();
    
    if(coordinates[a][b][0] == 0 && floodfill[a-1][b] == 0 && a > 0) {
      floodfill[a-1][b] = floodfill[a][b] + 1;
      oxy.enqueue({a-1,b});
      //setText(a-1,b, String(floodfill[a-1][b]));
    }
    if(coordinates[a][b][1] == 0 && floodfill[a][b+1] == 0 && b < maze_size-1) {
      floodfill[a][b+1] = floodfill[a][b] + 1;
      oxy.enqueue({a,b+1});
      //setText(a,b+1, String(floodfill[a][b+1]));
    }
    if(coordinates[a][b][2] == 0 && floodfill[a+1][b] == 0 && a < maze_size-1) {
      floodfill[a+1][b] = floodfill[a][b] + 1;
      oxy.enqueue({a+1,b});
      //setText(a+1,b, String(floodfill[a+1][b]));
    }
    if(coordinates[a][b][3] == 0 && floodfill[a][b-1] == 0 && b > 0) {
      floodfill[a][b-1] = floodfill[a][b] + 1;
      oxy.enqueue({a,b-1});
      //setText(a,b-1, String(floodfill[a][b-1]));
    }
  }
  
}


//hàm để quay xe theo hướng chỉ định
void change_direction (uint8_t d) {
    switch (d - o) {
        case 1:
            turnRight();
            break;
        case 2:
            // turnRight();
            // turnRight();
            turnAround();
            break;
        case 3:
            turnLeft();
            break;
        case -1:
            turnLeft();
            break;
        case -2:
            // turnLeft();
            // turnLeft();
            turnAround();
            break;
        case -3:
            turnRight();
            break;
    }

    o = d;
}

void Move() {
  //log(String(x) + ',' + String(y));
  
  for(int i=0; i<4; i++) {
    switch (priorityDirection[i]) {
      case 0:
        if(coordinates[x][y][0] == 0 && x > 0 && floodfill[x-1][y] < floodfill[x][y]) {
          change_direction(0);
          i = 4; //Đk thoát for
        }
        break;
      case 1:
        if(coordinates[x][y][1] == 0 && y < maze_size-1 && floodfill[x][y+1] < floodfill[x][y]) {
          change_direction(1);
          i = 4; //Đk thoát for
        }
        break;
      case 2:
        if(coordinates[x][y][2] == 0 && x < maze_size-1 && floodfill[x+1][y] < floodfill[x][y]) {
          change_direction(2);
          i = 4; //Đk thoát for
        }
        break;
      case 3:
        if(coordinates[x][y][3] == 0 && y > 0 && floodfill[x][y-1] < floodfill[x][y]) {
          change_direction(3);
          i = 4; //Đk thoát for
        }
        break;
    }
  }

  // if(coordinates[x][y][0] == 0 && x > 0 && floodfill[x-1][y] < floodfill[x][y]) {
  //   change_direction(0);
  // }else if(coordinates[x][y][1] == 0 && y < maze_size-1 && floodfill[x][y+1] < floodfill[x][y]) {
  //   change_direction(1);
  // }else if(coordinates[x][y][2] == 0 && x < maze_size-1 && floodfill[x+1][y] < floodfill[x][y]) {
  //   change_direction(2);
  // }else if(coordinates[x][y][3] == 0 && y > 0 && floodfill[x][y-1] < floodfill[x][y]) {
  //   change_direction(3);
  // }
  
  moveForward();
  
}
