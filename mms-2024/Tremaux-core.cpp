#include <iostream>
#include <string>

#include "API.h"

using namespace std;
//Biến toạ độ
int x = 0;
int y = 0;
int o = 0; //Biến chỉ hướng của xe

//Mảng 3 chiều lưu số lần qua 1 hướng của ngã rẽ
const int mapSizeX = 16;
const int mapSizeY = 16;

int tremaux[mapSizeX][mapSizeY][4]; //[16][16] là kích thước dài rộng của mê cung [4] là do 1 ngã rẽ có tối đa 4 hướng

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

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

//Hàm quay theo hướng chỉ định
void autoTurn(int direction) {
    while(((direction - o) & 0x03) != 0) {
        API::turnRight();
        o = (o+1) & 0x03;
        // log(to_string(o) + "," + to_string(direction));
    }
}

//Hàm tìm hướng có ít đánh dấu nhất
int minDirection() {
    int min = tremaux[x][y][0];
    int direction = 0;
    if(min > tremaux[x][y][1]) {
        min = tremaux[x][y][1];
        direction = 1;
    }
    if(min > tremaux[x][y][2]) {
        min = tremaux[x][y][2];
        direction = 2;
    }
    if(min > tremaux[x][y][3]) {
        min = tremaux[x][y][3];
        direction = 3;
    }
    return direction;
}

//Hàm thuật toán
void algorithm() {
    int wallCount = wallCheck();
    switch (wallCount) {
        case 3: //Nếu có 3 tường (tường nhìn thấy được - tức là không xét trường hợp tường ở đằng sau) thì quay đầu
            API::turnRight();
            API::turnRight();
            o = (o+2) & 0x03;
            break;
        case 2: //Nếu có 2 tường (tường nhìn thấy được) thì đi tiếp
            if(API::wallFront() == 1) {
                if(API::wallLeft() == 1) {
                    API::turnRight();
                    o = (o+1) & 0x03;
                }else{ //tổng số tường = 2 mà không có tường ở trái thì chắc chắn là tường ở phải (chỉ xét tường nhìn thấy được)
                    API::turnLeft();
                    o = (o-1) & 0x03;
                }
            }
            break;
        case 1: //Set các hướng có tường = 8 rồi đưa vào xử lý cùng với trường hợp không tường
            // tremaux[x][y][((o+2)&0x03)]++; //tăng hướng vừa vào lên 1 đơn vị
            if(API::wallFront() == 1) {
                tremaux[x][y][o] = 8;
            }else if(API::wallLeft() == 1) {
                tremaux[x][y][((o-1)&0x03)] = 8;
            }else if(API::wallRight() == 1) {
                tremaux[x][y][((o+1)&0x03)] = 8;
            }
        case 0:
            tremaux[x][y][((o+2)&0x03)]++; //tăng hướng vừa vào lên 1 đơn vị
            int randomDirection = 0;
            int sumTremaux = tremaux[x][y][0] + tremaux[x][y][1] + tremaux[x][y][2] + tremaux[x][y][3];
            
            if(sumTremaux == 1 || sumTremaux == 9) { //tức là lần đầu tiên đến ngã rẽ đó
                do {
                    randomDirection = rand() % 4;
                }while(tremaux[x][y][randomDirection] != 0);                
                autoTurn(randomDirection);
                tremaux[x][y][o]++;
            }else{
                if((tremaux[x][y][o] == 1) && (tremaux[x][y][((o+2)&0x03)] == 1)) { //Nếu đường trước mặt theo hướng đi hiện tại đã có 1 dấu, và đường đang đi hiện tại chỉ mới đánh dấu 1 lần, thì quay trở lại và đánh dấu đường ấy 2 vạch
                    autoTurn(((o+2)&0x03));
                    tremaux[x][y][o]++;
                }else{ //Chọn đường đi có ít vạch nhất, và nhớ đánh dấu đường ấy luôn
                    autoTurn(minDirection());
                    tremaux[x][y][o]++;
                }
            }
            break;
            

    }

    if(wallCount <= 1) {
        API::setText(x,y, to_string(tremaux[x][y][0]) + "," + to_string(tremaux[x][y][1]) + "," + to_string(tremaux[x][y][2]) + "," + to_string(tremaux[x][y][3]));
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
    //---------------------------------------------------

    API::moveForward();
    // API::setText(x,y,to_string(wallCheck()));
}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setWall(x,y,'s'); //Mặc định ban đầu có tường ở đăng sau của xe
    API::setText(x,y,to_string(wallCheck() + 1)); //wallCheck() + 1 là do có tường ở đăng sau xe

    while (!((x == mapSizeX/2 || x == (mapSizeX/2-1)) && (y == mapSizeY/2 || y == (mapSizeY/2-1)))) {
        // wallCheck();
        algorithm();
    }

}
