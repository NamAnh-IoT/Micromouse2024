#include <iostream>
#include <string>
#include <queue>
//#include <windows.h> //thư viện để dùng lệnh sleep (delay)

#include "API.h"

uint8_t coordinates[16][16][4];
uint8_t floodfill[16][16];
uint8_t x,y; //biến lưu toạ độ hiện tại của xe
uint8_t o = 1;
uint8_t a,b; //biến để chạy floodfill
bool reach_goal;

struct point {
	int x;
	int y ;
};

std::queue <point> oxy;

// 1 = đi tiến   (y tăng)
// 2 = quay phải (x tăng)
// 3 = đi lùi    (y giảm)
// 0 = quay trái (x giảm)
/* hướng của xe
 *		                y
 *	    1	      1         |
 *	 -2   2   0   2       |
 *	   -1	      3         O-----x
 *	    
 */


void log(const std::string& text) {
    std::cerr << text << std::endl;
}


void wall(uint8_t x, uint8_t y) {
	if(API::wallFront()){
		coordinates[x][y][o] = 1;
	}
	if(API::wallLeft()){
		coordinates[x][y][(o+3)%4] = 1;
	}
	if(API::wallRight()){
		coordinates[x][y][(o+1)%4] = 1;
	}
	
	
	//Do mỗi toạ độ đều lưu vị trí 4 hường tường nên sẽ bình chồng lên nhau, ví dụ ô 0,0 có tường bên phải thì ô 1,0 có tường ở bên trái
	if(coordinates[x][y][0] == 1 && x > 0 && coordinates[x-1][y][2] == 0) {
		coordinates[x-1][y][2] = 1;
	}
	if(coordinates[x][y][1] == 1 && y < 15 && coordinates[x][y+1][3] == 0) {
		coordinates[x][y+1][3] = 1;
	}
	if(coordinates[x][y][2] == 1 && x < 15 && coordinates[x+1][y][0] == 0) {
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


void show_wall(uint8_t x, uint8_t y) {
	if(coordinates[x][y][0] == 1) {
		API::setWall(x,y,'w');
	}
	if(coordinates[x][y][1] == 1) {
		API::setWall(x,y,'n');
	}
	if(coordinates[x][y][2] == 1) {
		API::setWall(x,y,'e');
	}
	if(coordinates[x][y][3] == 1) {
		API::setWall(x,y,'s');
	}
}

void goal_detemine() {
	
	if((x == 7 || x == 8) && (y == 7 || y == 8)) {
		reach_goal = 1;
	}
	
	if(reach_goal == 1) {
		for(uint8_t m = 7; m <= 8; m++) {
			for(uint8_t n = 7; n <= 8; n++) {
				for(uint8_t z = 0; z <= 3; z++) {
					coordinates[m][n][z] = 1;
				}
			}
		}
		/*
		coordinates[7][7][0] = 1;
		coordinates[7][8][0] = 1;
		coordinates[7][8][1] = 1;
		coordinates[8][8][1] = 1;
		coordinates[8][8][2] = 1;
		coordinates[8][7][2] = 1;
		coordinates[8][7][3] = 1;
		coordinates[7][7][3] = 1;
		*/
	}
	
	

	if(x == 7 && y == 7) {
		if(o == 1) {
			coordinates[7][7][3] = 0;
		}else if(o == 2) {
			coordinates[7][7][0] = 0;
		}
	}
	if(x == 7 && y == 8) {
		if(o == 3) {
			coordinates[7][8][1] = 0;
		}else if(o == 2) {
			coordinates[7][8][0] = 0;
		}
	}
	if(x == 8 && y == 8) {
		if(o == 3) {
			coordinates[8][8][1] = 0;
		}else if(o == 0) {
			coordinates[8][8][2] = 0;
		}
	}
	if(x == 8 && y == 7) {
		if(o == 0) {
			coordinates[7][8][2] = 0;
		}else if(o == 1) {
			coordinates[7][8][3] = 0;
		}
	}
	
	
	if(reach_goal == 1) {
		for(uint8_t m = 6; m <= 9; m++) {
			log("check lại wall của cột " + std::to_string(m));
			for(uint8_t n = 6; n <= 9; n++) {
				if(coordinates[x][y][0] == 1 && x > 0 && coordinates[x-1][y][2] == 0) {
					coordinates[x-1][y][2] = 1;
				}
				if(coordinates[x][y][1] == 1 && y < 15 && coordinates[x][y+1][3] == 0) {
					coordinates[x][y+1][3] = 1;
				}
				if(coordinates[x][y][2] == 1 && x < 15 && coordinates[x+1][y][0] == 0) {
					coordinates[x+1][y][0] = 1;
				}
				if(coordinates[x][y][3] == 1 && y > 0 && coordinates[x][y-1][1] == 0) {
					coordinates[x][y-1][1] = 1;
				}
				show_wall(m,n);

			}
		}
	}
		
}


void fill() {
	a = oxy.front().x;
	b = oxy.front().y;
	oxy.pop();
	
	if(coordinates[a][b][0] == 0 && floodfill[a-1][b] == 0 && a > 0) {
		floodfill[a-1][b] = floodfill[a][b] + 1;
		oxy.push({a-1,b});
		API::setText(a-1,b, std::to_string(floodfill[a-1][b]));
	}
	if(coordinates[a][b][1] == 0 && floodfill[a][b+1] == 0 && b < 15) {
		floodfill[a][b+1] = floodfill[a][b] + 1;
		oxy.push({a,b+1});
		API::setText(a,b+1, std::to_string(floodfill[a][b+1]));
	}
	if(coordinates[a][b][2] == 0 && floodfill[a+1][b] == 0 && a < 15) {
		floodfill[a+1][b] = floodfill[a][b] + 1;
		oxy.push({a+1,b});
		API::setText(a+1,b, std::to_string(floodfill[a+1][b]));
	}
	if(coordinates[a][b][3] == 0 && floodfill[a][b-1] == 0 && b > 0) {
		floodfill[a][b-1] = floodfill[a][b] + 1;
		oxy.push({a,b-1});
		API::setText(a,b-1, std::to_string(floodfill[a][b-1]));
	}
	
	/*
	if(coordinates[a][b][0] == 1){
		log("có tường ở bên trái ô " + std::to_string(a) + ',' + std::to_string(b));
	}
	if(coordinates[a][b][1] == 1){
		log("có tường ở bên trên ô " + std::to_string(a) + ',' + std::to_string(b));
	}
	if(coordinates[a][b][2] == 1){
		log("có tường ở bên phải ô " + std::to_string(a) + ',' + std::to_string(b));
	}
	if(coordinates[a][b][3] == 1){
		log("có tường ở bên dưới ô " + std::to_string(a) + ',' + std::to_string(b));
	}
	*/
	
	//Sleep(1);
}

void Flood_Fill() {
	
	//Đưa hết các ô về blank (giá trị = 0)
	for(uint8_t m = 0; m < 16; m++) {
		for(uint8_t n = 0; n < 16; n++) {
			floodfill[m][n] = 0;
			API::setText(m,n, "");
		}
	}
	
	//Set các ô đích = 1
	for(uint8_t m = 7; m <= 8; m++) {
		for(uint8_t n = 7; n <=8 ; n++) {
			floodfill[m][n] = 1;
			API::setText(m,n, std::to_string(floodfill[m][n]));
		}
	}
	
	
	point p = {a,b};
	oxy.push({7,7});
	oxy.push({7,8});
	oxy.push({8,8});
	oxy.push({8,7});
	
	while(!oxy.empty()) {
		fill();
	}
	
}


//hàm để quay xe theo hướng chỉ định
void change_direction (uint8_t direction) {
    switch (direction - o) {
        case 1:
            API::turnRight();
            break;
        case 2:
            API::turnRight();
            API::turnRight();
            break;
        case 3:
            API::turnLeft();
            break;
        case -1:
            API::turnLeft();
            break;
        case -2:
            API::turnLeft();
            API::turnLeft();
            break;
        case -3:
            API::turnRight();
            break;
    }

    o = direction;
}

void move() {
	//log(std::to_string(x) + ',' + std::to_string(y));
	


	//change_direction(2);
	//log("đã đổi hướng xong");


	if(coordinates[x][y][0] == 0 && x > 0 && floodfill[x-1][y] < floodfill[x][y]) {
		change_direction(0);
	}else if(coordinates[x][y][1] == 0 && y < 15 && floodfill[x][y+1] < floodfill[x][y]) {
		change_direction(1);
	}else if(coordinates[x][y][2] == 0 && x < 15 && floodfill[x+1][y] < floodfill[x][y]) {
		change_direction(2);
	}else if(coordinates[x][y][3] == 0 && y > 0 && floodfill[x][y-1] < floodfill[x][y]) {
		change_direction(3);
	}else {
		Flood_Fill();
		move();
	}

	
}
int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
	
	//Mặc định sẽ có tường ở sau lưng của xe nên set = 1;
	coordinates[0][0][3] = 1;
	Flood_Fill();	

	while(!reach_goal) {
		
		wall(x,y);
		//log("wall ok");
		show_wall(x,y);
		//log("show_wall ok");
		//Flood_Fill();
		//log("Flood_Fill ok");
		log(std::to_string(floodfill[x][y]));
		move();
		API::moveForward();

		//log("move ok");
		Oxy();
		//log("Oxy ok");
		goal_detemine();

	}
	
    Flood_Fill();
}
