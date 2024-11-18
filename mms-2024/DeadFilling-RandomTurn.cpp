#include <iostream>
#include <string>

#include "API.h"

using namespace std;

#define mazeSize 16
int coordinate[mazeSize][mazeSize][4]; //Mảng 3 chiều để lưu lại các tường
int x = 0;
int y = 0;
int D = 0; //Biến lưu hướng của xe
// 0 là thuận chiều Y, 1 là thuận chiều x, 2 là ngược chiều y, 3 là ngược chiều x
//     0
//     | 
//3----o----1
//     |
//     2


void log(const std::string& text) {
    std::cerr << text << std::endl;
}

int checkWall(int x, int y, int D) {
    char direction[4] = {'n','e','s','w'};
    int numberOfWall = 0;
    if(API::wallFront()) {
        API::setWall(x,y,direction[D & 0x03]);
        coordinate[x][y][D&0x03] = 1; 
        numberOfWall++;       
    }
    if(API::wallLeft()) {
        API::setWall(x,y,direction[(D-1) & 0x03]);
        coordinate[x][y][(D-1)&0x03] = 1;
        numberOfWall++;
    }
    if(API::wallRight()) {
        API::setWall(x,y,direction[(D+1) & 0x03]);
        coordinate[x][y][(D+1)&0x03] = 1;
        numberOfWall++;
    }

    //Do mảng 3 chiều nên mỗi toạ độ x,y sẽ đều có 1 chiều để lưu tường, phải đồng nhất tường lại thì mới đúng

    if(y+1 <= 15) coordinate[x][y+1][2] = coordinate[x][y][0];//Tường ở phía bên TRÊN ô x+y là tường ở phía bên DƯỚI của ô x-1,y
    if(x+1 <= 15) coordinate[x+1][y][3] = coordinate[x][y][1];//Tường ở phía bên PHẢI ô x+y là tường ở phía bên TRÁI của ô x-1,y
    if(y-1 >= 0)  coordinate[x][y-1][0] = coordinate[x][y][2];//Tường ở phía bên DƯỚI ô x+y là tường ở phía bên TRÊN của ô x-1,y
    if(x-1 >= 0)  coordinate[x-1][y][1] = coordinate[x][y][3];//Tường ở phía bên TRÁI ô x+y là tường ở phía bên PHẢI của ô x-1,y

    return numberOfWall;

}

void deadFilling(int a, int b) {
    if((coordinate[a][b][0] + coordinate[a][b][1] + coordinate[a][b][2] + coordinate[a][b][3]) == 3 && a >= 0 && a<= 15 && b >=0 && b <=15) {
        coordinate[a][b][0] = 1;
        coordinate[a][b][1] = 1;
        coordinate[a][b][2] = 1;
        coordinate[a][b][3] = 1;
        API::setWall(a,b,'n');
        API::setWall(a,b,'e');
        API::setWall(a,b,'s');
        API::setWall(a,b,'w');
    }

    // if(b+1 <= 15) coordinate[a][b+1][2] = coordinate[a][b][0];//Tường ở phía bên TRÊN ô a+b là tường ở phía bên DƯỚI của ô a-1,b
    // if(a+1 <= 15) coordinate[a+1][b][3] = coordinate[a][b][1];//Tường ở phía bên PHẢI ô a+b là tường ở phía bên TRÁI của ô a-1,b
    // if(b-1 >= 0)  coordinate[a][b-1][0] = coordinate[a][b][2];//Tường ở phía bên DƯỚI ô a+b là tường ở phía bên TRÊN của ô a-1,b
    // if(a-1 >= 0)  coordinate[a-1][b][1] = coordinate[a][b][3];//Tường ở phía bên TRÁI ô a+b là tường ở phía bên PHẢI của ô a-1,b
    
}

void autoRun() {
if(coordinate[x][y][D] == 1 && coordinate[x][y][(D-1)&0x03] == 1 && coordinate[x][y][(D+1)&0x03] == 1) {
        API::turnRight();
        API::turnRight();
        D = (D+2)&0x03;
    }else if(coordinate[x][y][D] == 1 && coordinate[x][y][(D-1)&0x03]) {
        API::turnRight();
        D = (D+1)&0x03;
    }else if(coordinate[x][y][D] == 1 && coordinate[x][y][(D+1)&0x03]) {
        API::turnLeft();
        D = (D-1)&0x03;
    }else if((coordinate[x][y][D] + coordinate[x][y][(D-1)&0x03] + coordinate[x][y][(D+1)&0x03]) == 1) {
        if(coordinate[x][y][D] == 1) {
            switch(rand() % 2) {
                case 0:
                    API::turnRight();
                    D = (D+1)&0x03;
                    break;
                case 1:
                    API::turnLeft();
                    D = (D-1)&0x03;
                    break;
                default:
                    log("wallfront Error (deadFilling)");
            }
        }else if(coordinate[x][y][(D+1)&0x03] == 1) {
            switch(rand() % 2) {
                case 0:
                    //Đi thẳng
                    break;
                case 1:
                    API::turnLeft();
                    D = (D-1)&0x03;
                    break;
                default:
                    log("wallfront Error (deadFilling)");
            }
        }else if(coordinate[x][y][(D-1)&0x03] == 1) {
            switch(rand() % 2) {
                case 0:
                    //Đi thẳng
                    break;
                case 1:
                    API::turnRight();
                    D = (D+1)&0x03;
                    break;
                default:
                    log("wallfront Error (deadFilling)");
            }
        }
    }

    deadFilling(x,y);
    deadFilling(x-1,y);
    deadFilling(x+1,y);

    if(y+1 <= 15) coordinate[x][y+1][2] = coordinate[x][y][0];//Tường ở phía bên TRÊN ô x+y là tường ở phía bên DƯỚI của ô x-1,y
    if(x+1 <= 15) coordinate[x+1][y][3] = coordinate[x][y][1];//Tường ở phía bên PHẢI ô x+y là tường ở phía bên TRÁI của ô x-1,y
    if(y-1 >= 0)  coordinate[x][y-1][0] = coordinate[x][y][2];//Tường ở phía bên DƯỚI ô x+y là tường ở phía bên TRÊN của ô x-1,y
    if(x-1 >= 0)  coordinate[x-1][y][1] = coordinate[x][y][3];//Tường ở phía bên TRÁI ô x+y là tường ở phía bên PHẢI của ô x-1,y

}


int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    // coordinate[0][0][2] = 1;
    // API::setWall(0,0,'s');

    for(int i=0; i<16; i++) {
        coordinate[i][15][0] = 1;
        coordinate[15][i][1] = 1;
        coordinate[i][0][2] = 1;
        coordinate[0][i][3] = 1;
        API::setWall(i,15,'n');
        API::setWall(15,i,'e');
        API::setWall(i,0,'s');
        API::setWall(0,i,'w');
    } 
    while (true) {
        // log(to_string(checkWall(x,y,D)));
        // checkCorridor();
        checkWall(x,y,D);
        // tremaux();
        autoRun();

        switch(D & 0x03) {
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
    }
}
