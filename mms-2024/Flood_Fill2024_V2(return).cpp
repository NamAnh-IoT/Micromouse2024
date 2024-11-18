#include <iostream>
#include <string>
#include <queue>
#include<windows.h>

#include "API.h"

using namespace std;

#define mazeSize 16
int coordinate[mazeSize][mazeSize][4]; //Mảng 3 chiều để lưu lại tường ở mọi ví trí trong mê cung
int floodFill_coordinate[16][16]; //Mảng để lưu tham số cho thuật toán floodfill
// 0 là thuận chiều Y, 1 là thuận chiều x, 2 là ngược chiều y, 3 là ngược chiều x
//     0
//     | 
//3----o----1
//     |
//     2

typedef struct point {
public:
    int x;
    int y;
} oxy;

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

void checkWall(int x, int y, int D) {
    char direction[4] = {'n','e','s','w'};
    if(API::wallFront()) {
        API::setWall(x,y,direction[D & 0x03]);
        coordinate[x][y][D&0x03] = 1;        
    }
    if(API::wallLeft()) {
        API::setWall(x,y,direction[(D-1) & 0x03]);
        coordinate[x][y][(D-1)&0x03] = 1;
    }
    if(API::wallRight()) {
        API::setWall(x,y,direction[(D+1) & 0x03]);
        coordinate[x][y][(D+1)&0x03] = 1;
    }

    //Do mảng 3 chiều nên mỗi toạ độ x,y sẽ đều có 1 chiều để lưu tường, phải đồng nhất tường lại thì mới đúng

    if(y+1 <= 15) coordinate[x][y+1][2] = coordinate[x][y][0];//Tường ở phía bên TRÊN ô x+y là tường ở phía bên DƯỚI của ô x-1,y
    if(x+1 <= 15) coordinate[x+1][y][3] = coordinate[x][y][1];//Tường ở phía bên PHẢI ô x+y là tường ở phía bên TRÁI của ô x-1,y
    if(y-1 >= 0)  coordinate[x][y-1][0] = coordinate[x][y][2];//Tường ở phía bên DƯỚI ô x+y là tường ở phía bên TRÊN của ô x-1,y
    if(x-1 >= 0)  coordinate[x-1][y][1] = coordinate[x][y][3];//Tường ở phía bên TRÁI ô x+y là tường ở phía bên PHẢI của ô x-1,y

}

void floodFill(int mode = 0) { //Tham số mode để chọn flood fill từ vị trí nào, 0 là floodfill từ đích, 1 là floodfill từ điểm xuất phát
    log("floodfill");
    for(int i=0; i<16;i++) {
        for(int k=0; k<16;k++) {
            floodFill_coordinate[i][k] = -1;
            API::setText(i,k,"");
        }
    }

    queue<oxy> floodFill_queue;
    //Các ô floodFill_coordinate[x][y] = -1 tức là chưa được xử lý
    //Set giá trị của đích đến là 0
    switch (mode) {
        case 0:
            floodFill_coordinate[7][8] = 0;
            floodFill_coordinate[8][8] = 0;
            floodFill_coordinate[8][7] = 0;
            floodFill_coordinate[7][7] = 0;

            floodFill_queue.push({7,8});
            floodFill_queue.push({8,8});
            floodFill_queue.push({8,7});
            floodFill_queue.push({7,7});
            break;
        case 1:
            floodFill_coordinate[0][0] = 0;
            floodFill_queue.push({0,0});
            break;
        default:
            break;
    }

    oxy point;
    while(!floodFill_queue.empty()) {
        point = floodFill_queue.front();
        floodFill_queue.pop();

        //Các điều kiện floodfill:
        //+ ô lân cận có chưa từng được xét qua
        //+ không có tường ở hiện tại và ô lân cận (điều kiện cần)
        //+ các ô nằm trong khoảng từ 0 - 15

        // if(point.x > 15 || point.x < 0 || point.y > 15 || point.y < 0) continue;

        //xét ô ở bên trên (hướng 'n')
        if(coordinate[point.x][point.y][0] == 0 && floodFill_coordinate[point.x][point.y+1] == -1 && point.y<15/* && floodFill_coordinate[point.x][point.y] < floodFill_coordinate[point.x][point.y+1] */) {
            floodFill_coordinate[point.x][point.y+1] = floodFill_coordinate[point.x][point.y] + 1;
            floodFill_queue.push({point.x, point.y+1});
            API::setText(point.x, point.y+1, to_string(floodFill_coordinate[point.x][point.y+1]));
        }
        //xét ô ở bên phải (hướng 'e')
        if(coordinate[point.x][point.y][1] == 0 && floodFill_coordinate[point.x+1][point.y] == -1 && point.x<15/* && floodFill_coordinate[point.x][point.y] < floodFill_coordinate[point.x+1][point.y] */) {
            floodFill_coordinate[point.x+1][point.y] = floodFill_coordinate[point.x][point.y] + 1;
            floodFill_queue.push({point.x+1, point.y});
            API::setText(point.x+1, point.y, to_string(floodFill_coordinate[point.x+1][point.y]));
        }
        //xét ô ở bên dưới (hướng 's')
        if(coordinate[point.x][point.y][2] == 0 && floodFill_coordinate[point.x][point.y-1] == -1 && point.y>0/* && floodFill_coordinate[point.x][point.y] < floodFill_coordinate[point.x][point.y-1] */) {
            floodFill_coordinate[point.x][point.y-1] = floodFill_coordinate[point.x][point.y] + 1;
            floodFill_queue.push({point.x, point.y-1});
            API::setText(point.x, point.y-1, to_string(floodFill_coordinate[point.x][point.y-1]));
        }
        //xét ô ở bên trái (hướng 'w')
        if(coordinate[point.x][point.y][3] == 0 && floodFill_coordinate[point.x-1][point.y] == -1 && point.x>0/* && floodFill_coordinate[point.x][point.y] < floodFill_coordinate[point.x-1][point.y] */) {
            floodFill_coordinate[point.x-1][point.y] = floodFill_coordinate[point.x][point.y] + 1;
            floodFill_queue.push({point.x-1, point.y});
            API::setText(point.x-1, point.y, to_string(floodFill_coordinate[point.x-1][point.y]));
        }

        // log(to_string(point.x) + "," + to_string(point.y));
        // Sleep(1);
    }   


}

int getMin(int x, int y) { //Hàm tìm giá trị nhỏ nhất trong số các ô lân cận, hỗ trợ cho hàm autoRun
    int min = 256;
    if(min > floodFill_coordinate[x][y+1] && coordinate[x][y][0] == 0) min = floodFill_coordinate[x][y+1];
    if(min > floodFill_coordinate[x+1][y] && coordinate[x][y][1] == 0) min = floodFill_coordinate[x+1][y];
    if(min > floodFill_coordinate[x][y-1] && coordinate[x][y][2] == 0) min = floodFill_coordinate[x][y-1];
    if(min > floodFill_coordinate[x-1][y] && coordinate[x][y][3] == 0) min = floodFill_coordinate[x-1][y];
    return min;
}

int autoRun(int x, int y, int D, int mode = 0) {
    int min = getMin(x,y);
    int direction = 0;
    log(to_string(coordinate[x][y][0]) + "," + to_string(coordinate[x][y][1]) + "," + to_string(coordinate[x][y][2]) + "," + to_string(coordinate[x][y][3]) + " | " + to_string(floodFill_coordinate[x][y+1]) + "," + to_string(floodFill_coordinate[x+1][y]) + "," + to_string(floodFill_coordinate[x][y-1]) + "," + to_string(floodFill_coordinate[x-1][y]));
    

    log(to_string(min));

    if(floodFill_coordinate[x][y] == 0) return D+4; //Nếu đến ô có giá trị 0 thì trả về một số ngoài khoảng (0-3) + 4 để có thể -4 để lấy được hướng hiện tại
    if(floodFill_coordinate[x][y] <= min) {
        // checkWall(x,y,D);
        switch (mode) {
            case 0:
                floodFill();
                log("ve dich");
                break;
            case 1:
                floodFill(1);
                log("ve diem xuat phat");
                break;
            default:
                break;
        }
        min = getMin(x,y);
        // autoRun(x,y,D);
    }
    if(min == floodFill_coordinate[x][y+1] && coordinate[x][y][0] == 0 && y+1 <= 15) {
        direction = 0;
        log("forward");
    }else if(min == floodFill_coordinate[x+1][y] && coordinate[x][y][1] == 0 && x+1 <= 15) {
        direction = 1;
        log("right");
    }else if(min == floodFill_coordinate[x][y-1] && coordinate[x][y][2] == 0 && y-1 >= 0) {
        direction = 2;
        log("back");
    }else if(min == floodFill_coordinate[x-1][y] && coordinate[x][y][3] == 0 && x-1 >= 0) {
        direction = 3;
        log("left");
    }

    // log(to_string(D&0x03) + "-" + to_string(direction) + "=" + to_string({(D-direction)&0x03}) + "  " + to_string(floodFill_coordinate[x][y]));
    switch((D-direction)&0x03) {
        case 0:
            break;
        case 1:
            API::turnLeft();
            break;
        case 2:
            API::turnLeft();
            API::turnLeft();
            break;
        case 3:
            API::turnRight();
            break;
        case -1:
            API::turnRight();
            break;
        case -2:
            API::turnRight();
            API::turnRight();
            break;
        case -3:
            API::turnLeft();
            break;
    }
    return direction;
}

void Oxy(int *x, int *y, int D) {
    switch(D & 0x03) {
            case 0:
                *y++;
                break;
            case 1:
                *x++;
                break;
            case 2:
                *y--;
                break;
            case 3:
                *x--;
                break;
        }
}

int main(int argc, char* argv[]) {
    int x = 0;
    int y = 0;
    // int D = 2147483648; //Giá trị ở giữa của int, chia 4 dư 0
    int D = 0;
    // 0 là thuận chiều Y, 1 là thuận chiều x, 2 là ngược chiều y, 3 là ngược chiều x

    log("Running...");
    API::setColor(0, 0, 'G');
    // API::setText(0, 0, "0,0");
    API::setWall(0,0,'s'); //Do ban đầu chắc chắn có tường ở đằng sau nên khởi tạo trước
    coordinate[0][0][2] = 1; //Do ban đầu chắc chắn có tường ở đằng sau nên khởi tạo trước
    
    floodFill();
    while (true) {

        checkWall(x, y, D);
        // floodFill();

        D = autoRun(x,y,D);
        if(D > 3) { //Nếu D ngoài khoảng (0-3) thì thoát vòng lặp
            break;
        }

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

        // Oxy(&x, &y, D);

        // log(to_string(point.x) + "," + to_string(point.y));
        API::moveForward();
    }

    floodFill(1);
    D = D-4;
    while (true) {

        checkWall(x, y, D);
        // floodFill();

        D = autoRun(x,y,D,1);
        if(D > 3) { //Nếu D ngoài khoảng (0-3) thì thoát vòng lặp
            break;
        }
        
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

    // log(to_string(point.x) + "," + to_string(point.y));
    API::moveForward();
    }

    floodFill(0);

}
