# 프로젝트명: BUS CATCHER
# 개요
본 프로젝트는 노약자, 임산부, 아동 등 버스를 놓치기 쉬운 교통약자들을 위한 버스 탑승 의사 알리미를 목적으로 합니다.

# 작동 과정
1. 교통약자들은 STM32 기반의 웨어러블 단말기를 장비합니다.
2. 손을 흔드는 제스처를 취하면 AI 학습 모델에 의해 단말기가 이를 인지합니다. 이후 단말기에서 BLE Beacon을 Advertising 모드로 송출합니다.
3. 버스 정류장의 버스 정보 시스템에 장착된 BLE 스캐너에서 Beacon 신호를 수신하고 해당 Beacon의 UID가 등록된 UID인 경우 AWS IoT로 데이터를 발행합니다.
4. AWS IoT에 구독중인 버스의 단말기에서 데이터를 수신하고 버스 기사에게 버스 정류장에 교통약자가 존재함을 알리고 주의하게 합니다.

# 구성도
![구성도](image/entire_system.png)

단말기 - BLE - 버스 정보 시스템 - AWS IoT - LTE - 버스

# 담당

## 하경준: BLE Beacon
- STM32보드 B-L4S5I-IOT01A
- Cube-MX
- [MCU/wearable_for_bus](https://github.com/SweetWeeds/shh2021_LiveinSIOR/tree/master/MCU/wearable_for_bus)

## 권한결: STM32Cube.AI, AWS IoT
- STM32보드 B-LS45I-IOT01A
- Cube.AI, AWS IoT
- [MCU/AI](https://github.com/SweetWeeds/shh2021_LiveinSIOR/tree/master/MCU/AI), [BUS/subscriber.py](https://github.com/SweetWeeds/shh2021_LiveinSIOR/blob/master/BUS/subscriber.py), [BUS_INFO/AWS_IoT](https://github.com/SweetWeeds/shh2021_LiveinSIOR/tree/master/BUS_INFO/AWS_IoT)

## 안현철: TTS, LCD
- 라즈베리파이4
- 구글 클라우드
- [BUS](https://github.com/SweetWeeds/shh2021_LiveinSIOR/tree/master/BUS)

## 김춘수: GUI
- 라즈베리파이4
- QT Designer
- [BUS_INFO/GUI](https://github.com/SweetWeeds/shh2021_LiveinSIOR/tree/master/BUS_INFO/GUI)

# 시연 영상
이미지를 클릭해주십시오.

[![](https://img.youtube.com/vi/4hIDGSzT3UQ/0.jpg)](https://www.youtube.com/watch?v=4hIDGSzT3UQ)
