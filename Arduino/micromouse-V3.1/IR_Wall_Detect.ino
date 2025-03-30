bool wallLeft() {
  if(analogRead(irR3) <= threshold3 + 10) {
    return 1;
  }else{
    return 0;
  }
}

bool wallFront() {
  if(analogRead(irR2) <= threshold2 + 10) {
    return 1;
  }else{
    return 0;
  }
}

bool wallRight() {
  if(analogRead(irR1) <= threshold1 + 10) {
    return 1;
  }else{
    return 0;
  }
}
