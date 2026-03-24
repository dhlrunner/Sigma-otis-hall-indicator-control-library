// ================================================================
// DCMIndicator - Library for Sigma/LG-Otis DCM-1XX Hall Indicator
// Created by dhlrunner (runner38)
// ================================================================

#pragma once
#include <Arduino.h>

#define MAX_FLOOR_NUM 99
#define MIN_FLOOR_NUM -9


class DCMIndicator {
public:
  //pinData: DATA핀
  //pinClk: CLK핀
  //pinEn: ENABLE핀
  //halfUs: 클럭 타이밍 (µs)
  //enPulseUs: ENABLE LOW 펄스폭 (µs)
  DCMIndicator(uint8_t pinData, uint8_t pinClk, uint8_t pinEn,
               uint16_t halfUs = 50, uint16_t enPulseUs = 50);

  void begin();

  void clear();

  //Send raw data frame
  //lamp: (6 bit) Overload(만원), Out of service(점검중), Arrow Up/Dn 등 
  //left: (11 bit) First dot matrix 
  //right: (8 bit) Second dot matrix
  void DCMIndicator::sendRawFrame(uint8_t lamp, uint16_t left, uint8_t right);

  //change timing param
  void DCMIndicator::setHalfUs(uint16_t halfUs);
  void DCMIndicator::setEnPulseUs(uint16_t enPulseUs);

  void DCMIndicator::setArrowUp();
  void DCMIndicator::setArrowDown();
  void DCMIndicator::setArrowClear();
  void DCMIndicator::setArrowAnimation(bool set);
  void DCMIndicator::setArrowAnimationFast(bool set);
  void DCMIndicator::setOverloadLamp(bool set);
  void DCMIndicator::setOutOfServiceLamp(bool set);
  void DCMIndicator::setFloor(int8_t floor);
  void DCMIndicator::setAscii(char left, char right);

private:
  //숫자 매핑
  static constexpr uint8_t _numbers[10] = {
    0x3F,
    0x06,
    0xDB,
    0xCF,
    0xE6,
    0xED,
    0xFD,
    0x27,
    0xFF,
    0xEF
  };

  uint8_t  _pinData;
  uint8_t  _pinClk;
  uint8_t  _pinEn;
  uint16_t _halfUs;
  uint16_t _enPulseUs;

  //두고쓰는 데이터들
  uint8_t _lampByte = 0;
  uint16_t _leftSegByte = 0; //11-bit
  uint8_t _rightSegByte = 0;
  
  void _pinAssert(uint8_t pin);
  void _pinDeassert(uint8_t pin);
  void _pinWrite(uint8_t pin, uint8_t bit);
  void _idle();
  void _shiftByte(uint8_t data);
  void _shiftBits(uint32_t data, uint8_t nbits);
  void _commit();
  void _clearDataBuffer();
  void _sendCurrentData();
  uint8_t _getAsciiCodeData(char c);
};
