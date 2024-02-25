#include <Adafruit_NeoPixel.h>
#include <GyverOLED.h>
#include <SPI.h>
#include <SD.h>

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS   5

double vectors[8][3] = {{20, 20, 20}, { -20, 20, 20}, { -20, -20, 20}, {20, -20, 20}, {20, 20, -20}, { -20, 20, -20}, { -20, -20, -20}, {20, -20, -20}};
double perspective = 100.0f;
int deltaX, deltaY, deltaZ, iter = 0;

static const int spiClk = 240000000; // 1 MHz
SPIClass SPISD(HSPI);

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
Adafruit_NeoPixel ledRGB(1, 48, NEO_GRB + NEO_KHZ800);





int8_t dr = 0;
int8_t dg = 0;
int8_t db = 0;
uint8_t r = 255, g = 0,  b = 0;

int X;
int Y;
int button = 0;

int menu_selector = 0;
bool menu_enter = false;

unsigned long timer;

void ledColor() {
  r += dr;
  g += dg;
  b += db;
  if(r == 255 && g == 0 && b == 0)
  {
      dr = 0; dg = 1; db = 0;
  }
  if(r == 255 && g == 255 && b == 0)
  {
      dr = -1; dg = 0; db = 0;
  }
  if(r == 0 && g == 255 && b == 0)
  {
      dr = 0; dg = 0; db = 1;
  }
  if(r == 0 && g == 255 && b == 255)
  {
      dr = 0; dg = -1; db = 0;
  }
  if(r == 0 && g == 0 && b == 255)
  {
      dr = 1; dg = 0; db = 0;
  }
  if(r == 255 && g == 0 && b == 255)
  {
      dr = 0; dg = 0; db = -1;
  }

  ledRGB.fill(ledRGB.Color(r, g, b));
  ledRGB.show();
}

void setup() {
  Serial.begin(115200);
  SPISD.begin();
  SPISD.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS); //SCLK, MISO, MOSI, SS
  pinMode(HSPI_CS, OUTPUT); //HSPI SS
  oled.init();
  Wire.setClock(800000L);
  ledRGB.begin();
  ledRGB.setBrightness(5);
  timer = millis();
  pinMode(16, INPUT_PULLUP);
  if (!SD.begin(HSPI_CS, SPISD))
  {
    Serial.println("Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");

  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}


void loop() {

  if (millis() - timer > 100) {
    ledColor();
    timer = millis();
    int X = analogRead(18);
    int Y = analogRead(17);
    Y = map(Y, 0, 4095, 5, -5);// Y джойст
    X = map(X, 0, 4095, 5, -5);// X джойст
    int button = !digitalRead(16);// кнопка

    if (X <= 1 && X >= -1) {X = 0;}
    if (X > 1) {X = 1;}
    if (X < -1) {X = -1;}
    
    if (Y <= 1 && Y >= -1) {Y = 0;}
    if (Y > 1) {Y = 1;}
    if (Y < -1) {Y = -1;}

    if (Y > 0) {menu_selector -= 1;}
    if (Y < 0) {menu_selector += 1;}
    if (menu_selector < 0) {menu_selector=0;}
    if (menu_selector > 7) {menu_selector=7;}

    if (button) {menu_enter = !menu_enter;}

    //Serial.print(X);
    //Serial.println(" X");
    //Serial.print(Y);
    //Serial.println(" Y");
    //Serial.print(button);
    //Serial.println(" Button");
  }
  
  

  
  if (menu_enter == false) {
    menu();
  } else {
    if (menu_selector == 0) {
      net();
    }
    if (menu_selector == 1) {
      bigBall();
    }
    if (menu_selector == 2) {
      lines();
    }
    if (menu_selector == 3) {
      cube();
    }
    if (menu_selector == 4) {
      ball();
    }
    if (menu_selector == 5) {
      bezier();
    }
    if (menu_selector == 6) {
      bezier2();
    }

  }
  

  //while (millis() - tmr < 5000) net();
  //while (millis() - tmr < 5000) bigBall();
  //while (millis() - tmr < 5000) lines();
  //while (millis() - tmr < 5000) circleModes();
  //while (millis() - tmr < 5000) ball();
  //while (millis() - tmr < 5000) bezier();
  //while (millis() - tmr < 5000) bezier2();
  
}

void cube() {
  oled.clear();
  oled.home();
  oled.print(1000 / timer);

  timer = millis();
  drawVectors();
  if (iter == 0) {
    deltaX = random(7) - 3;
    deltaY = random(7) - 3;
    deltaZ = random(7) - 3;
    iter   = random(250) + 5;
  }
  rotateX(deltaX);
  rotateY(deltaY);
  rotateZ(deltaZ);
  iter--;
  oled.update();
  timer = millis() - timer;
}

void menu() {
  oled.clear();
  oled.home();
  oled.println("Net");
  oled.println("BigBall");
  oled.println("Lines");
  oled.println("Cube");
  oled.println("Ball");
  oled.println("Bezier");
  oled.println("Bezier 2");
  oled.setCursorXY(80, menu_selector*8);
  oled.print("<--");
  oled.update();
}

void bezier2() {
  const byte amount = 3;
  static bool start = false;
  static int x[amount], y[amount];
  static int velX[amount], velY[amount];
  if (!start) {
    start = 1;
    for (byte i = 0; i < amount; i++) {
      x[i] = random(10, (128 - 1) * 10);
      y[i] = random(10, (64 - 1) * 10);
      velX[i] = random(5, 20);
      velY[i] = random(5, 20);
    }
  }
  oled.clear();
  int bez[(amount + 1) * 2];
  for (byte i = 0; i < amount; i++) {
    x[i] += velX[i];
    y[i] += velY[i];
    if (x[i] >= (128 - 1) * 10 || x[i] < 0) velX[i] = -velX[i];
    if (y[i] >= (64 - 1) * 10 || y[i] < 0) velY[i] = -velY[i];
    oled.dot(x[i] / 10, y[i] / 10, 1);
    bez[i * 2] = x[i] / 10;
    bez[i * 2 + 1] = y[i] / 10;
  }
  bez[amount * 2] = bez[0];
  bez[amount * 2 + 1] = bez[1];

  oled.bezier(bez, amount + 1, 8);
  oled.update();
}

void bezier() {
  int data[] = {0, 0, 128 / 2, 64 / 2, 0, 64 - 1};
  for (int i = 0; i < 128; i++) {
    oled.clear();
    data[0] = data[4] = 128 - i;
    data[2] = i;
    oled.bezier(data, 3, 7);
    oled.update();
  }
  for (int i = 128; i > 0; i--) {
    oled.clear();
    data[0] = data[4] = 128 - i;
    data[2] = i;
    oled.bezier(data, 3, 7);
    oled.update();
  }
}

void ball() {
  oled.clear();
  static int x, y;
  static int velX = 15, velY = 18;
  x += velX;
  y += velY;
  if (x >= (128 - 1) * 10 || x < 0) velX = -velX;
  if (y >= (64 - 1) * 10 || y < 0) velY = -velY;

  oled.circle(x / 10, y / 10, 3);
  oled.circle(x / 10 + 1, y / 10 + 1, 3);
  oled.circle(x / 10 + 1, y / 10, 3);
  oled.circle(x / 10, y / 10 + 1, 3);
  oled.update();
  delay(10);
}

void net() {
  const byte radius = 2;
  const byte amount = 20;
  static bool start = false;
  static int x[amount], y[amount];
  static int velX[amount], velY[amount];
  if (!start) {
    start = 1;
    for (byte i = 0; i < amount; i++) {
      x[i] = random(10, (128 - 1) * 10);
      y[i] = random(10, (64 - 1) * 10);
      velX[i] = random(2, 9);
      velY[i] = random(2, 9);
    }
  }
  oled.clear();
  for (byte i = 0; i < amount; i++) {
    x[i] += velX[i];
    y[i] += velY[i];
    if (x[i] >= (128 - 1 - radius) * 10 || x[i] < radius * 10) velX[i] = -velX[i];
    if (y[i] >= (64 - 1 - radius) * 10 || y[i] < radius * 10) velY[i] = -velY[i];
    oled.circle(x[i] / 10, y[i] / 10, radius);
  }

  for (int i = 0; i < amount; i++) {
    for (int j = 0; j < amount; j++) {
      if (i != j && dist(x[i] / 10, y[i] / 10, x[j] / 10, y[j] / 10) < 35) oled.line(x[i] / 10, y[i] / 10, x[j] / 10, y[j] / 10);
    }
  }
  oled.update();
}

int dist(int x1, int y1, int x2, int y2) {
  int lx = (x2 - x1);
  int ly = (y2 - y1);
  return (sqrt(lx * lx + ly * ly));
}

void bigBall() {
  oled.clear();
  byte radius = 10;
  static int x = (128 / 2) * 10, y = (64 / 2) * 10;
  static int velX = 17, velY = 9;
  static bool fillFlag = 0;
  x += velX;
  y += velY;
  if (x >= (128 - radius) * 10 || x < radius * 10) {
    velX = -velX;
    fillFlag = !fillFlag;
  }
  if (y >= (64 - radius) * 10 || y < radius * 10) {
    velY = -velY;
    fillFlag = !fillFlag;
  }

  oled.circle(x / 10, y / 10, radius, fillFlag ? OLED_STROKE : OLED_FILL);
  oled.update();
}

void lines() {
  oled.clear();
  for (byte i = 0; i < 128 - 1; i += 3) {
    oled.line(0, 0, i, 64);
    oled.update();
  }
  for (int i = 64 - 1; i >= 0 ; i -= 3) {
    oled.line(0, 0, 128, i);
    oled.update();
  }
  delay(100);

  oled.clear();
  for (int i = 128 - 1; i > 0; i -= 3) {
    oled.line(128 - 1, 0, i, 64);
    oled.update();
  }
  for (int i = 64 - 1; i > 0; i -= 3) {
    oled.line(128 - 1, 0, 0, i);
    oled.update();
  }
  delay(100);
}

void circleModes() {
  oled.clear();
  oled.fill(255);
  oled.createBuffer(64 - 20, 32 - 20, 64 + 20, 32 + 20, 255);
  oled.circle(64, 32, 20, OLED_CLEAR);
  oled.sendBuffer();
  oled.update();
  delay(800);

  oled.clear();
  oled.createBuffer(64 - 20, 32 - 20, 64 + 20, 32 + 20);
  oled.circle(64, 32, 20, OLED_FILL);
  oled.sendBuffer();
  oled.update();
  delay(800);

  oled.clear();
  oled.createBuffer(64 - 20, 32 - 20, 64 + 20, 32 + 20);
  oled.circle(64, 32, 20, OLED_STROKE);
  oled.sendBuffer();
  oled.update();
  delay(800);
}

int translateX(double x, double z) {
  return (int)((x + 64) + (z * (x / perspective)));
}

int translateY(double y, double z) {
  return (int)((y + 32) + (z * (y / perspective)));
}

void rotateX(int angle) {
  double rad, cosa, sina, Yn, Zn;
  rad = angle * PI / 180;
  cosa = cos(rad);
  sina = sin(rad);
  for (int i = 0; i < 8; i++) {
    Yn = (vectors[i][1] * cosa) - (vectors[i][2] * sina);
    Zn = (vectors[i][1] * sina) + (vectors[i][2] * cosa);
    vectors[i][1] = Yn;
    vectors[i][2] = Zn;
  }
}

void rotateY(int angle) {
  double rad, cosa, sina, Xn, Zn;
  rad = angle * PI / 180;
  cosa = cos(rad);
  sina = sin(rad);
  for (int i = 0; i < 8; i++)
  {
    Xn = (vectors[i][0] * cosa) - (vectors[i][2] * sina);
    Zn = (vectors[i][0] * sina) + (vectors[i][2] * cosa);
    vectors[i][0] = Xn;
    vectors[i][2] = Zn;
  }
}

void rotateZ(int angle) {
  double rad, cosa, sina, Xn, Yn;
  rad = angle * PI / 180;
  cosa = cos(rad);
  sina = sin(rad);
  for (int i = 0; i < 8; i++) {
    Xn = (vectors[i][0] * cosa) - (vectors[i][1] * sina);
    Yn = (vectors[i][0] * sina) + (vectors[i][1] * cosa);
    vectors[i][0] = Xn;
    vectors[i][1] = Yn;
  }
}

void drawVectors() {
  oled.line(translateX(vectors[0][0], vectors[0][2]), translateY(vectors[0][1], vectors[0][2]), translateX(vectors[1][0], vectors[1][2]), translateY(vectors[1][1], vectors[1][2]));
  oled.line(translateX(vectors[1][0], vectors[1][2]), translateY(vectors[1][1], vectors[1][2]), translateX(vectors[2][0], vectors[2][2]), translateY(vectors[2][1], vectors[2][2]));
  oled.line(translateX(vectors[2][0], vectors[2][2]), translateY(vectors[2][1], vectors[2][2]), translateX(vectors[3][0], vectors[3][2]), translateY(vectors[3][1], vectors[3][2]));
  oled.line(translateX(vectors[3][0], vectors[3][2]), translateY(vectors[3][1], vectors[3][2]), translateX(vectors[0][0], vectors[0][2]), translateY(vectors[0][1], vectors[0][2]));
  oled.line(translateX(vectors[4][0], vectors[4][2]), translateY(vectors[4][1], vectors[4][2]), translateX(vectors[5][0], vectors[5][2]), translateY(vectors[5][1], vectors[5][2]));
  oled.line(translateX(vectors[5][0], vectors[5][2]), translateY(vectors[5][1], vectors[5][2]), translateX(vectors[6][0], vectors[6][2]), translateY(vectors[6][1], vectors[6][2]));
  oled.line(translateX(vectors[6][0], vectors[6][2]), translateY(vectors[6][1], vectors[6][2]), translateX(vectors[7][0], vectors[7][2]), translateY(vectors[7][1], vectors[7][2]));
  oled.line(translateX(vectors[7][0], vectors[7][2]), translateY(vectors[7][1], vectors[7][2]), translateX(vectors[4][0], vectors[4][2]), translateY(vectors[4][1], vectors[4][2]));
  oled.line(translateX(vectors[0][0], vectors[0][2]), translateY(vectors[0][1], vectors[0][2]), translateX(vectors[4][0], vectors[4][2]), translateY(vectors[4][1], vectors[4][2]));
  oled.line(translateX(vectors[1][0], vectors[1][2]), translateY(vectors[1][1], vectors[1][2]), translateX(vectors[5][0], vectors[5][2]), translateY(vectors[5][1], vectors[5][2]));
  oled.line(translateX(vectors[2][0], vectors[2][2]), translateY(vectors[2][1], vectors[2][2]), translateX(vectors[6][0], vectors[6][2]), translateY(vectors[6][1], vectors[6][2]));
  oled.line(translateX(vectors[3][0], vectors[3][2]), translateY(vectors[3][1], vectors[3][2]), translateX(vectors[7][0], vectors[7][2]), translateY(vectors[7][1], vectors[7][2]));
}