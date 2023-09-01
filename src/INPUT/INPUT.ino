// 아두이노끼리 연결 |        4x4 키패드       | 가변저항 |   스위치 2개   |와 연결된 아두이노의 것
//   2,3핀 사용   | 4,5,6,7,8,9,10,11핀 사용|  A0사용 |  12,13핀 사용

#include <SoftwareSerial.h>
#include <Wire.h>                             
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>            

SoftwareSerial mySerial(2, 3); // mySerial 아두이노끼리 통신할 객체 생성 (2,3핀사용)
const byte ROWS = 4;
const byte COLS = 4;
int yesButton = 12;
int noButton = 13;           
                      //        도    레   미    파    솔   라    시   도   
                      // 4옥타브 262, 294, 330, 349, 392, 440, 494, 523
                      // 5옥타브 523, 587, 659, 698, 784, 880, 988, 1047
                      // 6옥타브 1047, 1175, 1319, 1397, 1568, 1760, 1976, 2093
                      // 7옥타브 2093, 2349, 2637, 2794, 3136, 3520, 3951, 4186
char keys[ROWS][COLS] = {   // 키 번호 세팅 
  {'1', '2', '3', '4'}, 
  {'5', '6', '7', '8'},
  {'9', 'A', 'B', 'C'}, // 10 이상부터는 mySerial.write를 할 때 char 타입으로 전송하기 위해 문자 사용
  {'D', 'E', 'F', 'G'}  // mySerial.read 함수만을 쓰기 위해 두 자리수를 사용하지 않음 (두 자리수 사용 시 복잡한 함수 사용해야 함)
};

byte rowPins[ROWS] = {7,6,5,4}; // R1, R2, R3, R4 단자가 연결된 핀 번호 (4,5,6,7,8,9,11핀 사용)
byte colPins[COLS] = {8,9,10,11}; // C1, C2, C3, C4 단자가 연결된 핀 번호

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // 키패드 객체 생성

void setup () {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(yesButton, INPUT); // 버튼 스위치 yes 버튼 12핀
  pinMode(noButton, INPUT); // 버튼 스위치 no 버튼 13핀
}

void loop () {
  char key = keypad.getKey(); // 사용자가 4x4 키패드에서 누른 값을 받아옴
  int yesOn = digitalRead(yesButton); // yes에 해당하는 버튼 스위치에서 눌러진 값을 받아옴
  int noOn = digitalRead(noButton); // no에 해당하는 버튼 스위치에서 눌러진 값을 받아옴
  if(yesOn) mySerial.write('Y');  // yes 버튼 스위치가 눌리면 output 담당 아두이노에게 'Y'를 보냄
  else if(noOn) mySerial.write('N');  // no 버튼 스위치가 눌리면 output 담당 아두이노에게 'N'를 보냄
  
  if (key) {   // F = 15번 키
    if (key == 'F') { // 엔터 역할을 함 
      int gabyun = analogRead(A0);
      int halfOfMax = 1023/2; // 가변저항 값을 반으로 나눠서 왼쪽과 오른쪽으로 구분할 수 있게 함 (모드 선택)
      if (gabyun < halfOfMax) mySerial.write('S');  
                                                    // 왼쪽이면 점수제 모드가 되게끔 output 아두이노에게 'S'라고 보냄
      else mySerial.write('T'); 
                                // 오른쪽이면 타임어택 모드가 되게끔 output 아두이노에게 'T'라고 보냄
    }
    else { // 사용자가 게임할 때 누르는 값, '1','2','3'... 의 정보를 output 아두이노에게 보냄
      mySerial.write(key);
    }
  }
}