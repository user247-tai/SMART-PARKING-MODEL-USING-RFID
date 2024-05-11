// khoi tao cac thu vien su dung
#include <SPI.h>
#include <MFRC522.h>
// hai thu vien tren dung cho viec doc the
#include <Wire.h>
#include <Servo.h> // thu vien giao tiep voi servo
#include <LiquidCrystal_I2C.h> // thu vien giao tiep voi LCD

// Khai bao cac chan
int cambien1 = 8, cambien2 = 2; // 2 chan cam bien vat can
#define RST_PIN         9       // chan rst cua MRC522
#define SS_PIN          10      // chan SDA cua MRC522
const int RED_PIN = 3;    // led đỏ
const int GREEN_PIN = 4; // led xanh lá
const int BLUE_PIN = 5; // led xanh dương

LiquidCrystal_I2C lcd(0x3F,16,2); // khoi tao LCD 16x2 
Servo myServo1, myServo2; // Khai bao servo
MFRC522 mfrc522(SS_PIN, RST_PIN); // khai bao module MRC522

// Khai bao cac bien su dung trong chuong trinh
int UID[4], i;
// the trang: 229 135 163 172
// the xanh: 141 56 255 49
int ID[4] = {229, 135, 163, 172}; // UID cua Thẻ nhận dạng tín hiệu
int socho  = 5, dem = 0, pre1 = 1, pre2 = 1, cho = 0; // so cho toi da la 5
bool check = 0, successRead = 0;

// ham in ra man hinh LCD
void printLCD(){
  lcd.setCursor(0,0);
  lcd.print("So cho trong: ");
  lcd.print(socho - dem);
  lcd.setCursor(0,1);
  lcd.print("Xin moi vao");
}

void sservo1(){ // mo cong vao
  myServo1.write(180);
  delay(2000);
  myServo1.write(90);
}
void sservo2(){ // mo cong ra
  myServo2.write(0);
  delay(2000);
  myServo2.write(90);
}

// ham doc the - xuat ra serial monitor
bool docthe()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())  // kiem tra xem co phai the moi de doc khong
  { 
    return; 
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) // doc thong tin trong the
  {  
    return;  
  }
  
  Serial.print("UID của thẻ: ");   
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  { 
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");   
    UID[i] = mfrc522.uid.uidByte[i];
    Serial.print(UID[i]);
  }

  Serial.println("   ");
  mfrc522.PICC_HaltA();  // ngung doc the lai
  mfrc522.PCD_StopCrypto1(); // dung MFRC522
  check = (UID[0] == ID[0]) && (UID[1] == ID[1]) && (UID[2] == ID[2]) && (UID[3] == ID[3]); // kiem tra xem UID co bang ID khong
  return 1;
}

// so trang thai cua may trang thai nay chi la 1, dung bien dem xac dinh so xe ra/vao, dung dieu kien check cung voi dieu kien cac cam bien de tang/giam bien dem
void trangthai() 
{
  int gtcambien1 = digitalRead(cambien1);
  int gtcambien2 = digitalRead(cambien2);
  if (gtcambien1 == 0 && pre1 == 1 && gtcambien2 == 0 && pre2 == 1)
  {
    sservo2();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Moi quet the!");
    while (!successRead){
      successRead = docthe();
      if (cho == 500) break;
      cho = cho + 1;
    }
    if (check){
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("The dung!");
      display_led_R();
      sservo1();
      delay(100);
      check = 0;
    }
    else{
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("The sai!");
      delay(500);
    }
    
    successRead = 0;
cho = 0;
  }
  else if(gtcambien1 == 0 && pre1 == 1 && dem < socho)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Moi quet the!");
    while (!successRead){
      successRead = docthe();
      if (cho == 500) break;
      cho = cho + 1;
    }
    if (check){
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("The dung!");
      display_led_R();
      dem = dem + 1;
      sservo1();
      delay(200);
      check = 0;
    }
    else{
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("The sai!");
      delay(500);
    }
    
    successRead = 0;
    cho = 0;
  }
  else if(gtcambien2 == 0 && pre2 == 1)
  {
    display_led_R();
    dem = dem - 1;
    sservo2();
    delay(200); 
  }
  pre1 = gtcambien1;
  pre2 = gtcambien2;
  UID[0] = 0;
  UID[1] = 0;
  UID[2] = 0;
  UID[3] = 0;
}

// ham in ra output chinh
void print_output()
{
  if (dem == socho){
    lcd.setCursor(0,0);
    lcd.print("Da het cho chua");
    lcd.setCursor(0,1);
    lcd.print("           ");
  }
  else printLCD();
}

void display_led_R() // sang den mau do
{
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void display_led_G() // sang den mau xanh la
{
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
}

// ham khoi tao cac tin hieu
void setup() {
  // khoi tao tin hieu 2 cam bien
  pinMode(cambien1, INPUT);
  pinMode(cambien2, INPUT);
  myServo1.attach(6); // chan servo vao
  myServo2.attach(7); // chan servo ra
  // khoi tao vao bat den ngoai cho LCD
  lcd.init();
  lcd.backlight();
  // khoi tao cac chan LED RGB
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  // khoi dong man hinh serial
  Serial.begin(9600);
  // Khoi tao giao thuc SPI va module MFRC522
  SPI.begin();    
  mfrc522.PCD_Init();
}

// ham chay chinh
void loop() {
  if (dem == socho) display_led_R(); // den do khi day cho
  else display_led_G(); // nguoc lai den xanh
  trangthai();
  print_output();

}