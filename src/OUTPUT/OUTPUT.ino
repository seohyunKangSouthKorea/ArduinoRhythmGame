// 아두이노끼리 연결 |   lcd   |   led 2개   |   부저   |와 연결된 아두이노의 것
//   2,3핀 사용   | A4,A5사용 |  5,6핀 사용 |  8핀 사용 |

#include <SoftwareSerial.h>
#include <Wire.h>                             
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>   

LiquidCrystal_I2C lcd(0x27,16,2); // lcd 객체 생성               
SoftwareSerial mySerial(2, 3); // 아두이노 간 연결을 위한 mySerial 객체 생성 (2,3핀 사용)

String gameName = "Hold the note";
String start0 = "Game  Name";
String start1 = "1. SELECT MODE";
String start2 = "L: SCORE";
String start3 = "R: TIMEATTACK";
String start4 = "2. PRESS 15 KEY";
                //     1    2    3     4    5    6    7    8     9     A     B     C
const int melody[] = {523, 587, 659, 698, 784, 880, 988, 1047, 1175, 1319, 1397, 1568};
                //     도   레   미   파    솔   라   시    도    레    미    파    솔
const int dT = 50;  // 딜레이 초 50/1000초 (50ms)
const int dur = 50; // 부저에서 나오는 소리 지속 시간 50/1000초 == (50ms) 

int greenLed = 11; // 초록 led핀
int redLed = 13;  // 빨간 led핀

int buzzer = 12;  // 부저 핀
int first = 0;

// 개굴개굴 개구리
String ch[6] = {    
  "555555333331",   // 계이름에 맞는 키 번호 저장
  "222223434321",   // 게이머가 누른 키가 키 번호와 맞는지 확인
  "222343233365",   // 첫 행 출력도 함
  "222343232365",   
  "888885323456",
  "555556788888"
};

// 반짝반짝 작은 별
String ch2[3] = {
  "11556654433221",   // 키 번호 저장
  "55443325544332",
  "11556654433221"
};

// 초기 화면 출력을 위한 함수 
void firstComment() {
  lcd.print(start1);
  delay(1500);
  lcd.clear();
  
  if (first == 0){ // 모드 종류는 게임 실행 시에만 알려 주고 재시작 부분에서는 보여 주지 않음
    lcd.print(start2);
    lcd.setCursor(0,1);
    lcd.print(start3);
    delay(3000);
    lcd.clear();
  }
  first++;
  lcd.print(start4);  // 15 키를 누를 때까지 계속 출력

  char tmp;

  while (1) {
    tmp = mySerial.read(); // 읽어들인 가변저항에 의해 생성된 정보를 가져옴
    if (tmp == 'S' || tmp == 'T') {
      lcd.clear();
      break;
    }
  }

  if (tmp == 'S') scoreMode();
  else if (tmp == 'T') timeAttackMode();
}

// 게임 재시작 여부를 결정하기 위한 함수 
void restartOrShutDown ()
{
  // 게임이 다 끝나면 다시 시작할 건지 물어봄
  lcd.print("Restart?? Y or N");   
  while (mySerial.available() == 0) {} // 입력 대기

  char ans;
  while (1) {
    ans = mySerial.read(); // y나 n이 아닌 다른 문자를 입력한 경우를 체크
    if (ans == 'Y' || ans == 'N') break; // 제대로 누르면 벗어남
  }
  lcd.clear();

  // 재시작을 선택한 경우 (yes 누른 경우)
  if (ans == 'Y') {
    digitalWrite(greenLed, HIGH);
    delay(3000);
    digitalWrite(greenLed, LOW);
  }
  else if (ans == 'N') { // shut down하고자 하는 경우 (no 누른 경우) 
    digitalWrite(redLed, HIGH);
    lcd.setCursor(5,0);
    lcd.print("!!END!!");
    tone(buzzer, melody[7], dur);
    delay(100);
    tone(buzzer, melody[4], dur);
    delay(100);
    tone(buzzer, melody[2], dur);
    delay(100);
    tone(buzzer, melody[0], dur);
    delay(100);   
    delay(2600);
    digitalWrite(redLed, LOW);
    lcd.clear();
    lcd.noBacklight();  // led와 lcd 모두 지우고 끔
    while (1) {}  // void loop()를 돌 수 없게끔 내용이 비어 있는 무한 루프를 돌림
  }
}

// 버튼별로 음을 다르게 출력하기 위한 함수
void toneOutputByPressingButton (char tmp) {
  switch (tmp) {   // 도 레 미 파 솔 라 시 도 레 미 파 솔까지 표현 가능
    case '1': 
      tone(buzzer, melody[0], dur); // 도
      delay(dT);
      break;
    case '2':
      tone(buzzer, melody[1], dur); // 레
      delay(dT);
      break;
    case '3': 
      tone(buzzer, melody[2], dur); // 미
      delay(dT);
      break;
    case '4': 
      tone(buzzer, melody[3], dur); // 파
      delay(dT);
      break;
    case '5': 
      tone(buzzer, melody[4], dur); // 솔
      delay(dT);
      break;
    case '6': 
      tone(buzzer, melody[5], dur); // 라
      delay(dT);
      break;
    case '7': 
      tone(buzzer, melody[6], dur); // 시
      delay(dT);
      break;
    case '8': 
      tone(buzzer, melody[7], dur); // 도
      delay(dT); 
      break;
    case '9':
      tone(buzzer, melody[8], dur); // 레
      delay(dT);
      break;
    case 'A':
      tone(buzzer, melody[9], dur); // 미
      delay(dT);
      break;
    case 'B':
      tone(buzzer, melody[10], dur); // 파
      delay(dT);
      break;
    case 'C':
      tone(buzzer, melody[11], dur); // 솔
      delay(dT);
      break;  
    }
  noTone(buzzer);
}

// 점수제 모드 
void scoreMode () {
  int score = 360; // 72*5 360, 만점을 360으로 설정
  int minusScore = 5; // 틀릴 때마다 5점씩 깎음 

  for (int i=0; i<6; i++){  // 한 음절이 6번 반복됨
    lcd.print(ch[i]);   // 입력해야 할 번호를 첫 번째 행에 출력함  
    for (int j=0; j<12; j++){  // 한 음절에 12개의 음
      while (mySerial.available() == 0) {} // c언어에서는 입력 대기를 해 주었지만 아두이노에서는 입력 대기를 해 주지 않음
                                        // 입력 대기를 위한 중요한 설정!! scanf() 기능이 없어서 직접 만들어야 함
      char tmp = mySerial.read(); // 사용자가 누른 key 값을 읽어옴
      toneOutputByPressingButton(tmp); // 번호에 해당하는 음을 정해 주는 함수
      
      if (ch[i][j] == tmp) { // key 값과 출력된 번호가 맞으면   
        lcd.setCursor(j,1);
        lcd.write('O'); // 바로 아래 O 출력
        delay(100);
      }

      else {
        lcd.setCursor(j,1);
        lcd.write('X'); // 아랫줄에 틀리면 X
        digitalWrite(redLed, HIGH);  // 틀리면 빨간색 led가 켜짐
        delay(100);
        digitalWrite(redLed, LOW);
        score -= minusScore;  // 틀릴 때마다 점수 차감 (-5)
      }
    }  
    lcd.clear(); 
  }

  if (score == 360) {  // 한 번도 틀리지 않으면
    lcd.setCursor(1,0);
    lcd.print("Full Combo!!!!");  // 풀 콤보 출력
    delay(2000);
    lcd.clear();
  }

  else {  // 게임 클리어 시
    lcd.setCursor(4,0);
    lcd.print("Clear!!!");
    delay(1000);
    lcd.clear();
  }

  lcd.setCursor(3,0);
  lcd.print("Score: "); // 점수 출력
  lcd.setCursor(11,0);
  lcd.print(score);
  delay(2000);
  lcd.clear();

  restartOrShutDown();  // 게임 재시작 여부를 물어보는 함수
}

// 틀렸을 때 부저음 출력을 위한 함수
void BeepBeep() {
  tone(buzzer, melody[11], dur); // 틀릴 때마다 부저음 출력
  digitalWrite(redLed, HIGH); // 빨간색 led 출력
  delay(100); 
  tone(buzzer, melody[11], dur);
  delay(100); // 위 과정 2번씩 반복됨
  noTone(buzzer);
  digitalWrite(redLed, LOW);
}

// 타임 어택 모드 
void timeAttackMode() {
  // 쓸모없는 음수를 쓰지 않기 위해 unsigned 사용
  int count = 0; // 틀린 횟수
  unsigned long firstTime = 0, dec = 0, lastTime = 0; // -21억~21억 => unsinged로 0~42억까지 쓸 수 있음
  firstTime = millis();

  for (int i=0; i<3; i++) {     // 한 음절이 3번 반복됨
    lcd.print(ch2[i]);   // 입력해야 할 번호를 첫 번째 행에 출력함 
    for (int j=0;j<14;j++){  // 한 음절에 14개의 음
      while (mySerial.available() == 0) {}  // c언어에서는 입력 대기를 해 주었지만 아두이노에서는 입력 대기를 해 주지 않음
                                            // 입력 대기를 위한 중요한 설정!! scanf() 기능이 없어서 직접 만들어야 함
      char tmp = mySerial.read();
      toneOutputByPressingButton(tmp);

      if (ch2[i][j] != tmp) {  // 틀리면 맞을 때까지 기다려야 함
        BeepBeep(); // 틀렸을 때 부저가 울림
        count++;  // 틀린 횟수 증가

        while (1) {
          while (mySerial.available() == 0) {} // 입력 대기 
          tmp = mySerial.read();  // 사용자가 입력한 key 값 불러옴
          toneOutputByPressingButton(tmp);

          if (ch2[i][j] == tmp) break;  // 맞으면 벗어날 수 있도록 함
          else if (ch2[i][j] != tmp) {
            BeepBeep();
            count++;
          }
        }
      }
      
      lcd.scrollDisplayLeft(); // 맞으면 한 칸씩 왼쪽으로 스크롤
    }  
    lcd.clear(); // 한 음절이 끝나면 초기화
  }

  lastTime = millis();  // 실질적으로 타임 어택 모드가 끝나는 데 걸린 시간

  dec = lastTime - firstTime; // 코드의 최종 실행 시간
  unsigned long result = dec/1000;  // ms 단위를 s로 바꿔 줌 

  if (count == 0) { // 한 번도 틀리지 않으면
    lcd.setCursor(1,0);
    lcd.print("Full Combo!!!!"); // 풀 콤보 출력
    delay(2000);
    lcd.clear();
  }

  else { // 게임 클리어 시 
    lcd.setCursor(4,0);
    lcd.print("Clear!!!");
    delay(1000);
    lcd.clear();
  }

  lcd.print(result/3600); // 1시간 = 3600초
  result -= (result/3600)*3600;
  lcd.setCursor(2,0);
  lcd.print("h "); // 시간을 나타냄

  lcd.setCursor(4,0);
  lcd.print(result/60); // 1분 = 60초
  result -= (result/60)*60;
  lcd.setCursor(6,0);
  lcd.print("m "); // 분을 나타냄

  lcd.setCursor(8,0);
  lcd.print(result);  // 1초 = 1초
  lcd.setCursor(10,0);
  lcd.print("s"); // 초를 나타냄
  delay(4000);
  lcd.clear();

  restartOrShutDown();  // 게임 재시작 여부를 물어봄
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  lcd.setCursor(0,0); // x,y
  pinMode(greenLed, OUTPUT); // 초록 불 y가 왔을 때
  pinMode(redLed, OUTPUT);  // 빨간 불 n가 왔을 때

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  // 시작 음 출력
  tone(buzzer, melody[0], dur);
  delay(100);
  tone(buzzer, melody[2], dur);
  delay(100);
  tone(buzzer, melody[4], dur);
  delay(100);
  tone(buzzer, melody[7], dur);
  delay(100);
  lcd.print(start0);
  lcd.setCursor(1,1);
  lcd.print(gameName);
  delay(3000);
  lcd.clear();
}

void loop () {
  firstComment();
}