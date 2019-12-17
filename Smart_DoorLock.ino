#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Servo.h>

SoftwareSerial FG(3, 2);
SoftwareSerial BT(4, 5);
Servo myservo;

int state = 0;

// ====================================================피에조 부저
int piezo = 6;
int numTones = 3;
int oTones[] = {261, 330, 392};
int cTones[] = {392, 330, 261};

// ====================================================초음파 센서
int trig = 10;
int echo = 11;

// ====================================================RGB LED
int red = 9;
int green = 13;
int blue = 12;

// ====================================================초음파 반응하는 거리
double dis;

// ====================================================초음파 반응 시간
double dur;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FG);

// ====================================================setup
void setup()
{
  Serial.begin(9600);
  // 스위치 핀 설정
  pinMode(7, INPUT);
  // 피에조 부저 핀 설정
  pinMode(piezo, OUTPUT);
  // 서보모터 핀 설정
  myservo.attach(8);
  //LED
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  //초음파
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

// ====================================================loop
void loop()
{
  //  Serial.println(digitalRead(7));
  if (digitalRead(7) == HIGH) {
    state = 1 - state;
  }
  if (state == 1) {
    FGset();
    delay(1000);
  } else {
    BTset();
    delay(1000);
  }
}

// ====================================================지문인식센서 함수
void FGset() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\n\nAdafruit finger detect test");
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  // ====================================================초음파센서
  digitalWrite(trig, HIGH);
  delay(100);
  digitalWrite(trig, LOW);

  // ====================================================초음파센서 거리를 cm로 변환 과정
  dur = pulseIn(echo, HIGH);
  dis = dur / 29.0 / 2.0;
  Serial.print(dis);
  Serial.println("cm");

  if (dis < 15) { //사람이 있을때
    digitalWrite(red, HIGH);
  } else { //사람이 없을 때
    digitalWrite(red, LOW);
  }

  getFingerprintIDez();
  delay(50);
}

// =======================================================블루투스센서 함수
void BTset() {
  Serial.begin(9600);

  BT.begin(9600);
  Serial.println("BT mode");
  // ====================================================초음파센서
  digitalWrite(trig, HIGH);
  delay(100);
  digitalWrite(trig, LOW);

  // ====================================================초음파센서 거리를 cm로 변환 과정
  dur = pulseIn(echo, HIGH);
  dis = dur / 29.0 / 2.0;
  Serial.print(dis);
  Serial.println("cm");

  if (dis < 15) { // ====================================================사람이 있을때
    digitalWrite(red, HIGH);
  } else { // ====================================================사람이 없을 때
    digitalWrite(red, LOW);
  }
  if (BT.available() > 0) {
    char ch = (char)BT.read();
    Serial.println(ch);
    // ==================================================== 값이 o 일때
    if (ch == 'o') {
      for (int i = 0; i < numTones; i++) {
        tone(piezo, oTones[i]);
        delay(400);
      }
      noTone(piezo);
      myservo.write(90);
      digitalWrite(green, HIGH);
    } else if ( ch == 'c') { //====================================================값이 c 일때
      for(int i = 0; i<numTones; i++){
        tone(piezo,cTones[i]);
        delay(400);
      }
      noTone(piezo);
      myservo.write(0);
      digitalWrite(green, LOW);
    }
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// ==================================================== 지문인식 성공시 실행 함수
int getFingerprintIDez() {

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  // ==================================================== 지문인식 성공후 문 열릴때
  myservo.write(90);
  digitalWrite(green, HIGH);
  for(int i = 0; i<numTones; i++){
    tone(piezo,oTones[i]);
    delay(400);
  }
  noTone(piezo);
  delay(5000);

  // ==================================================== 지문인식 성공후 문 닫힐때
  myservo.write(0);
  digitalWrite(green, LOW);
  for(int i = 0; i<numTones; i++){
    tone(piezo,cTones[i]);
    delay(400);
  }
  noTone(piezo);

  return finger.fingerID;
}
