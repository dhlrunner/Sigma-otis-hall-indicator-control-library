#include "DCMIndicator.h"

#define PIN_DATA 2
#define PIN_CLK 3
#define PIN_EN 4

DCMIndicator dcm(PIN_DATA, PIN_CLK, PIN_EN);

void setup(){
  dcm.begin();
  dcm.clear();
  dcm.setFloor(1);
  delay(2000);
}

void loop(){
  int maxFloor   = 100;
  int pivotFloor = 90;

  int startDelay = 1400;
  int minDelay   = 60;

  dcm.setArrowUp();
  delay(1000);
  dcm.setArrowAnimationFast(false);
  dcm.setArrowAnimation(true);
  delay(2000);
  dcm.setArrowAnimationFast(true);

  //1부터 100(PH)까지
  for (int i = 1; i <= maxFloor; i++) {
    dcm.setFloor((uint8_t)i);

    float t;
    int curdel;

    if (i <= pivotFloor) {

      t = (float)(i - 1) / (pivotFloor - 1);   // 0 ~ 1
      curdel = startDelay - (int)((startDelay - minDelay) * pow(t, 0.05));
    } 
    else {
      //감속 시작
      t = (float)(i - pivotFloor) / (maxFloor - pivotFloor); // 0 ~ 1
      curdel = minDelay + (int)((startDelay - minDelay) * (1.0 - cos(t * PI / 2.0)));
    }
    if(i > 93) //93층부터 화살표 애니메이션 느리게 설정
      dcm.setArrowAnimationFast(false);
      
    if(i == 100){
      dcm.setAscii('P', 'H'); //PH
      dcm.setArrowAnimation(false); 
    }
    delay(curdel);
  }


  delay(2000);
  dcm.setArrowDown(); //아래 화살표 설정
  delay(4000);

  dcm.setArrowAnimation(true);
  delay(2000);
  dcm.setArrowAnimationFast(true);


  int topFloor      = 99;
  int bottomFloor   = -9;
  int slowStartDown = 2;
  //99에서 -9까지
  for (int floor = topFloor; floor >= bottomFloor; floor--) {
    if(floor == 0)
      dcm.setAscii(NULL, 'L');
    else
      dcm.setFloor((uint8_t)floor);

    float t;
    int curdel;

    if (floor >= slowStartDown) {
      t = (float)(topFloor - floor) / (topFloor - slowStartDown);
      curdel = startDelay - (int)((startDelay - minDelay) * pow(t, 0.05));
    } 
    else {
      //감속 시작
       dcm.setArrowAnimationFast(false);

      t = (float)(slowStartDown - floor) / (slowStartDown - bottomFloor);
      curdel = minDelay + (int)((startDelay - minDelay) * (1.0 - cos(t * PI / 2.0)));
    }

    delay(curdel);
  }

    dcm.setArrowAnimation(false);
    delay(2000);
    dcm.setArrowClear();
    //dcm.setOutOfServiceLamp(true);
    delay(40000000);

}


