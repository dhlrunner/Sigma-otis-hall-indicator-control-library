#include "DCMIndicator.h"

constexpr uint8_t DCMIndicator::_numbers[10];

DCMIndicator::DCMIndicator(uint8_t pinData, uint8_t pinClk, uint8_t pinEn,
                           uint16_t halfUs, uint16_t enPulseUs)
  : _pinData(pinData), _pinClk(pinClk), _pinEn(pinEn),
    _halfUs(halfUs), _enPulseUs(enPulseUs)
{
  
}

//초기화
void DCMIndicator::begin() {
  //INPUT으로 둬서 DCM 보드측에서 일단 24V 풀업 유지하게 함 (이게 정석인지는 모름)
  pinMode(_pinData, INPUT);
  pinMode(_pinClk,  INPUT);
  pinMode(_pinEn,   INPUT);
  delayMicroseconds(_halfUs * 4);
  setArrowAnimationFast(false); //기본 설정이 on이므로 일단 끄기
  _idle();
}


void DCMIndicator::_pinAssert(uint8_t pin) {
  //그라운드로 풀다운
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void DCMIndicator::_pinDeassert(uint8_t pin) {
  //핀 해제
  pinMode(pin, INPUT);
}

void DCMIndicator::_pinWrite(uint8_t pin, uint8_t bit) {
  if (bit) _pinDeassert(pin);
  else     _pinAssert(pin);
}

//Set to idle
void DCMIndicator::_idle() {
  _pinDeassert(_pinEn);
  _pinAssert(_pinClk);    //CLK LOW
  _pinAssert(_pinData);   //DATA LOW
}


void DCMIndicator::_shiftBits(uint32_t data, uint8_t nbits) {
  for (uint8_t i = 0; i < nbits; i++) {
    uint8_t bit = (data >> (nbits - 1 - i)) & 1;  //MSB first

    _pinWrite(_pinData, bit);
    delayMicroseconds(_halfUs);

    _pinDeassert(_pinClk);          //CLK HIGH
    delayMicroseconds(_halfUs);
    _pinAssert(_pinClk);            //CLK LOW
    delayMicroseconds(_halfUs / 2);
  }
}

//EN LOW로 주면 커밋됨 (받은 데이터 도트에 표시) 
void DCMIndicator::_commit() {
  //CLK 복귀
  _pinAssert(_pinClk);
  delayMicroseconds(_halfUs * 2);

  _pinAssert(_pinEn);
  delayMicroseconds(_enPulseUs);
  _pinDeassert(_pinEn);
}

void DCMIndicator::_clearDataBuffer(){
  _lampByte = 0;
  _leftSegByte = 0;
  _rightSegByte = 0;
}

void DCMIndicator::_sendCurrentData(){
  sendRawFrame(_lampByte, _leftSegByte, _rightSegByte);
}

//ascii to hex bit convert helper
uint8_t DCMIndicator::_getAsciiCodeData(char c) {
  switch (c) {
    
    case '0': return 0x3F;
    case '1': return 0x30;
    case '2': return 0xDB;
    case '3': return 0xCF;
    case '4': return 0xE6;
    case '5': return 0xED;
    case '6': return 0xFD;
    case '7': return 0x27;
    case '8': return 0xFF;
    case '9': return 0xEF;
    
    case 'A': return 0xF7;
    case 'B': return 0x490;
    case 'C': return 0x39;
    case 'D': return 0x89;
    case 'E': return 0xF9;
    case 'F': return 0xF1;
    case 'G': return 0xBD;
    case 'H': return 0xF6;
    case 'I': return 0x01;
    case 'J': return 0x1E;
    case 'K': return 0x05;
    case 'L': return 0x38;
    case 'N': return 0x09;
    case 'O': return 0x11;
    case 'P': return 0xF3;
    case 'Q': return 0x21;
    case 'R': return 0xB7;
    case 'S': return 0x41;
    case 'T': return 0x81;
    case 'U': return 0x3E;
    case 'V': return 0x0F;
    case 'W': return 0x13;
    case 'X': return 0x0A;
    case 'Y': return 0x12;
    case 'Z': return 0x22;

    case 'a': return 0xDF;
    case 'b': return 0xFC;
    case 'c': return 0xD8;
    case 'd': return 0xDE;
    case 'e': return 0xBB;
    case 'f': return 0x71;
    case 'g': return 0x0C;
    case 'h': return 0xF4;
    case 'i': return 0x14;
    case 'j': return 0x24;
    case 'k': return 0x44;
    case 'm': return 0x84;
    case 'n': return 0x0D;
    case 'o': return 0xDC;
    case 'p': return 0x28;
    case 'q': return 0x48;
    case 'r': return 0xD0;
    case 's': return 0x50;
    case 't': return 0x90;
    case 'u': return 0x1C;
    case 'v': return 0xA0;
    case 'x': return 0xA4;
    case 'y': return 0x64;
    case 'z': return 0x2C;
    default:  return 0x00;
  }
}

//send data frame to device
void DCMIndicator::sendRawFrame(uint8_t lamp, uint16_t left, uint8_t right) {
  _idle();
  delayMicroseconds(_halfUs * 2);

  _shiftBits(lamp,  6);
  _shiftBits(left,  11);
  _shiftBits(right, 8);

  _commit();
  _idle();
}

void DCMIndicator::setArrowUp(){
  _lampByte |= (1 << 0);
  _lampByte &= ~(1 << 1);
  _sendCurrentData();
}

void DCMIndicator::setArrowDown(){
  _lampByte |= (1 << 1);
  _lampByte &= ~(1 << 0);
  _sendCurrentData();
}

void DCMIndicator::setArrowClear(){
  _lampByte &= ~(1 << 0);
  _lampByte &= ~(1 << 1);
  _sendCurrentData();
}

void DCMIndicator::setArrowAnimation(bool set){
  if (set) {
    _lampByte |= (1 << 4);
  } 
  else {
    _lampByte &= ~(1 << 4);
  }
  _sendCurrentData();
}


void DCMIndicator::setArrowAnimationFast(bool set){
  if (!set) {
    _lampByte |= (1 << 5);
  } 
  else {
    _lampByte &= ~(1 << 5);
  }
  _sendCurrentData();
}

//Set Overload Lamp (만원)
void DCMIndicator::setOverloadLamp(bool set){
  if (set) {
    _lampByte |= (1 << 2);
  } 
  else {
    _lampByte &= ~(1 << 2);
  }
  _sendCurrentData();
}

//Set out of service lamp (점검중)
void DCMIndicator::setOutOfServiceLamp(bool set){
  if (set) {
    _lampByte |= (1 << 3);
  } 
  else {
    _lampByte &= ~(1 << 3);
  }
  _sendCurrentData();
}

//Set floor number to dot
//MAX 99 MIN -9, other value is ignored
//negative number will be converted "Bx" (-1 -> B1)
void DCMIndicator::setFloor(int8_t floor){
  if(floor > MAX_FLOOR_NUM) return;
  if(floor < MIN_FLOOR_NUM) return;
  if(floor < 0){
    _leftSegByte = 0b11111111111;
    _rightSegByte = _numbers[abs(floor) % 10];
  }
  else{
    _leftSegByte = (floor / 10) == 0 ? 0x00 : _numbers[floor / 10];
    _rightSegByte = _numbers[floor % 10];
  }
  _sendCurrentData();
}

//WIP, Set Ascii code to dot
//NULL to skip setting
void DCMIndicator::setAscii(char left, char right){
  if(left != NULL) _leftSegByte = _getAsciiCodeData(left);
  if(right != NULL) _rightSegByte = _getAsciiCodeData(right);
  _sendCurrentData();
}

//All lamp off
void DCMIndicator::clear() {
  _clearDataBuffer();
  _sendCurrentData();
}

//change timing param
void DCMIndicator::setHalfUs(uint16_t halfUs) {
  _halfUs = halfUs;
}

void DCMIndicator::setEnPulseUs(uint16_t enPulseUs) {
  _enPulseUs = enPulseUs;
}
