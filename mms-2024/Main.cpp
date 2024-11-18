#include <iostream>
#include <string>

#include "API.h"
uint8_t coordinates[16][16][4];
uint8_t tremaux[16][16][4];
uint8_t x,y;
uint8_t o = 1; //orientation - Hướng của xe hiện tại
uint8_t last_o = 0; //last orientation - Hướng của xe 1 step trước
uint8_t last_o_one = 0; //last orientation - Hướng của xe 2 step trước
uint8_t last_o_two = 0; //last orientation - Hướng của xe 3 step trước
uint8_t last_o_three = 0; //last orientation - Hướng của xe 4 step trước
bool reach_goal = 0;
// 1 = đi tiến   (y tăng)
// 2 = quay phải (x tăng)
// 3 = đi lùi    (y giảm)
// 0 = quay trái (x giảm)
/* hướng của xe
 *		              y
 *		1	    1         |
 *	 -2   2   0   2       |
 *	   -1	    3         O-----x
 *	    
 */

void log(const std::string& text) {
    std::cerr << text << std::endl;
}



void wall() {
	if(API::wallFront()){
		coordinates[x][y][o] = 1;
	}
	if(API::wallLeft()){
		coordinates[x][y][(o+3)%4] = 1;
	}
	if(API::wallRight()){
		coordinates[x][y][(o+1)%4] = 1;
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


void show_wall() {
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

void goal_determine() {
	if(o != last_o && o != last_o_one && o != last_o_two && last_o != last_o_one && last_o != last_o_two && last_o_one != last_o_two && last_o_two == last_o_three) {
		reach_goal = 1;
	}
	
	log(std::to_string(o) + ',' + std::to_string(last_o) + ',' + std::to_string(last_o_one) + ',' + std::to_string(last_o_two) + ',' + std::to_string(last_o_three));

}


void move() {
	last_o_three = last_o_two;
	last_o_two = last_o_one;
	last_o_one = last_o;
	last_o = o;
	
	//tường ở bên trái và phía trước 
	if(API::wallLeft() && API::wallFront() && !API::wallRight()) {
		API::turnRight();
		o = (o + 1) % 4;
	}
	//tường ở bên trái và bên phải
	if(API::wallLeft() && !API::wallFront() && API::wallRight()) {
		//Đi thẳng
	}
	//tường ở phía trước và bên phải
	if(!API::wallLeft() && API::wallFront() && API::wallRight()) {
		API::turnLeft();
		o = (o + 3) % 4;
	}
	//Đường cụt
	if(API::wallLeft() && API::wallFront() && API::wallRight()) {
		API::turnRight();
		API::turnRight();
		o = (o + 2) % 4;
	}
	
	//Các trường hợp chỉ có 1 tường --------------------------------
	
	if(coordinates[x][y][1] + coordinates[x][y][2] + coordinates[x][y][3] + coordinates[x][y][0] == 1) {
		//TH1: tường ở trước mặt
		if(!API::wallLeft() && API::wallFront() && !API::wallRight()) {
			//Đánh dấu lối đi vào là đã đi 1 lần
			tremaux[x][y][(o+2)%4] = tremaux[x][y][(o+2)%4] + 1;
			if(tremaux[x][y][(o+2)%4] == 3) {
				tremaux[x][y][(o+2)%4] = 2;
				o = (o+2)%4;
				API::turnRight();
				API::turnRight();
			}else{
				if(tremaux[x][y][(o+1)%4] == 1 && tremaux[x][y][(o+3)%4] == 1 && tremaux[x][y][(o+2)%4] < 2) {
					tremaux[x][y][(o+2)%4] = tremaux[x][y][(o+2)%4] + 1;
					o = (o+2)%4;
					API::turnRight();
					API::turnRight();
				}else{
					//check xem bên phải đã đi qua chưa
					//o = (o + 1) % 4;
					//tremaux[x][y][o] = tremaux[x][y][o] + 1;
					if(tremaux[x][y][(o+1)%4] > tremaux[x][y][(o+3)%4]) {
						o = (o + 3) % 4;
						API::turnLeft();
					}else{
						API::turnRight();
						//tremaux[x][y][o] = tremaux[x][y][o] + 1;
					}
					
					tremaux[x][y][o] = tremaux[x][y][o] + 1;
				}
				
			}
			
		
		//TH2: tường ở bên trái
		} else if(API::wallLeft() && !API::wallFront() && !API::wallRight()) {
			//Đánh dấu lối đi vào là đã đi 1 lần
			tremaux[x][y][(o+2)%4] = tremaux[x][y][(o+2)%4] + 1;
			if(tremaux[x][y][(o+2)%4] == 3) {
				tremaux[x][y][(o+2)%4] = 2;
				o = (o+2)%4;
				API::turnRight();
				API::turnRight();
			}else{
				//check xem đằng trước đã đi qua chưa
				//o = o;
				if(tremaux[x][y][o] == 1 && tremaux[x][y][(o+2)%4] < 2) {
					o = (o+2)%4;
					API::turnRight;
					API::turnRight;
				}else{
					
					if(tremaux[x][y][o] > tremaux[x][y][(o+1)%4]) {
						o = (o + 1) % 4;
						API::turnRight();
					}else{
						//Đi thẳng
					}
					
					tremaux[x][y][o] = tremaux[x][y][o] + 1;
				}
			}
		//TH3: tường ở bên phải
		} else if(!API::wallLeft() && !API::wallFront() && API::wallRight()) {
			//Đánh dấu lối đi vào là đã đi 1 lần
			tremaux[x][y][(o+2)%4] = tremaux[x][y][(o+2)%4] + 1;
			if(tremaux[x][y][(o+2)%4] == 3) {
				tremaux[x][y][(o+2)%4] = 2;
				o = (o+2)%4;
				API::turnRight();
				API::turnRight();
			}else{
				//check xem đằng trước đã đi qua chưa
				//o = o;
				if(tremaux[x][y][o] > tremaux[x][y][(o+3)%4]) {
					o = (o+3)%4;
					API::turnLeft();
				}else{
					//Đi thẳng
				}
				
				tremaux[x][y][o] = tremaux[x][y][o] + 1;
			}
		}
	}
	

	
	//thuật toán Trémaux
	/*
	if(coordinates[x][y][0] + coordinates[x][y][1] + coordinates[x][y][2] + coordinates[x][y][3] < 2){
		log(std::to_string(coordinates[x][y][0] + coordinates[x][y][1] + coordinates[x][y][2] + coordinates[x][y][3]));
		
		tremaux[x][y][(o+2)%4] = tremaux[x][y][(o+2)%4] + 1;
		if(API::wallFront() && tremaux[x][y][(o+1)%4] == 0) {
			API::turnRight();
			o = (o + 1) % 4;
		}
		if(API::wallFront() && tremaux[x][y][(o+1)%4] == tremaux[x][y][(o+3)%4] && tremaux[x][y][(o+1)%4] >= tremaux[x][y][o]) {
			API::turnRight();
			API::turnRight();
			o = (o + 2) % 4;
		}
		if(API::wallLeft()) {
			if(tremaux[x][y][o] < tremaux[x][y][(o+1)%4] && tremaux[x][y][o] < tremaux[x][y][(o+2)%4]) {
				//Đi thẳng
			}
			if(tremaux[x][y][(o+1)%4] < tremaux[x][y][o] && tremaux[x][y][(o+1)%4] < tremaux[x][y][(o+2)%4]) {
				API::turnRight();
				o = (o + 1) % 4;
			}
			if(
		}
		tremaux[x][y][o] = tremaux[x][y][o] + 1;
		
	}
	*/
	
	
	//bám trái
	/*
	if (!API::wallLeft()) {
        API::turnLeft();
		o = (o + 3) % 4;
    }
    while (API::wallFront()) {
		API::turnRight();
		o = (o + 1) % 4;
    }
	*/

	
    API::moveForward();
}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    while (reach_goal != 1) {
		wall();
		show_wall();

		//API::setText(x, y, std::to_string(x) + ',' + std::to_string(y) + ',' + std::to_string(coordinates[x][y][0]) + ',' + std::to_string(coordinates[x][y][1]) + ',' + std::to_string(coordinates[x][y][2]) + ',' + std::to_string(coordinates[x][y][3]));
	
		move();
		
		
		if(tremaux[x][y][0] + tremaux[x][y][1] + tremaux[x][y][2] + tremaux[x][y][3]) {
			API::setText(x, y, std::to_string(tremaux[x][y][1]) + ',' + std::to_string(tremaux[x][y][2]) + ',' + std::to_string(tremaux[x][y][3]) + ',' + std::to_string(tremaux[x][y][0]));
		}else{
			API::setText(x, y, "O");
		}
		goal_determine();
		
		
		Oxy();
		
    }
}

