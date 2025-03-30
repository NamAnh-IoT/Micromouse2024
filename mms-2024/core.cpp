/*Chú ý*/
//core chỉ bao gồm hàm check tường và hàm xác định toạ độ của xe

#include <iostream>
#include <string>

#include "API.h"

using namespace std;
//Biến toạ độ
int x = 0;
int y = 0;
int o = 0; //Biến chỉ hướng của xe


//Hàm check tường và hiển thị tường
int wallCheck() {
    char direction[4] = {'n', 'e', 's', 'w'}; //quy ước 0 - hướng bắc, 1 - hướng đông, 2 - hướng nam, 3 - hướng tây
    int count = 0;
    if(API::wallFront() == 1) {
        API::setWall(x,y,direction[o]);
        count++;
    }
    if(API::wallLeft() == 1) {
        API::setWall(x,y, direction[(o-1) & 0x03]);
        count++;
    }
    if(API::wallRight() == 1) {
        API::setWall(x,y, direction[(o+1) & 0x03]);
        count++;
    }
    return count; //trả về số lượng tường xung quang xe
}

//Hàm thuật toán
void algorithm() {
    if (!API::wallLeft()) {
        API::turnLeft();
        o = (o-1) & 0x03;
    }
    while (API::wallFront()) {
        API::turnRight();
        o = (o+1) & 0x03;
    }
    //thao tác với toạ độ khi biết hướng di chuyển của xe
    switch(o) {
        case 0:
            y++;
            break;
        case 1:
            x++;
            break;
        case 2:
            y--;
            break;
        case 3:
            x--;
            break;
    }
    API::moveForward();
    API::setText(x,y,to_string(x) + "," + to_string(y));
}

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    while (true) {
        wallCheck();
        algorithm();
    }
}
