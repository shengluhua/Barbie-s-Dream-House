#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// TFT display and SD card will share the hardware SPI interface.
#define SD_CS    4  // Chip select line for SD card
#define SD_MOSI 11
#define SD_MISO 12
#define SD_SCK  13
#define TFT_CS   14  // Chip select line for TFT display
#define TFT_DC   15 // Data/command line for TFT
#define TFT_RST  16  // Reset line for TFT (or connect to +5V)

#define cs 10
#define dc 9
#define rst 8



Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
Adafruit_ST7735 text = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
SoftwareSerial retft_Serial(5, 6); // RX, TX

#define BUFFPIXEL 20 // 缩小缓存大小

// 使用 C 风格字符串替代 String
const char* clothes[] = {
  "black.bmp",
  "blue.bmp",
  "chistmas.bmp",
  "countryside.bmp",
  "fish.bmp",
  "green.bmp",
  "pink.bmp",
  "purple.bmp",
  "school.bmp",
  "white.bmp",
  "work.bmp"
};

const int numClothes = sizeof(clothes) / sizeof(clothes[0]);  // 图片总数
const int displayTime = 60 * 1000;  // 图片显示时间（60秒，单位为毫秒）

const char* currentImage = "cover.bmp";  // 当前显示的图片

unsigned long previousMillis = 0;
unsigned long interval = 3000; // 3秒

int lineY = 57;  // 从 Hi, Barbie! 下面开始
int lineHeight = 16 * 2;  // 每行文本占用的高度（textSize为2时）

// 处理每200毫秒换一次背景色的逻辑
unsigned long lastMillis = 0;           // 用于延时 200ms 的计时
unsigned long imageDisplayMillis = 0;    // 用于图片显示的计时
int imageDisplayDuration = 60000;        // 图片显示时间，单位：毫秒
unsigned long displayAllMillis = 0;      // 用于控制滚动显示服装的计时
int displayAllDuration = 3000;           // 每张图片显示 3 秒

void setup(void) {
  pinMode(12, INPUT); // Set SD's MISO IO State, VERY IMPORTANT!
  Serial.begin(9600);
  retft_Serial.begin(9600);

  // Initialize 1.8" TFT
  tft.initR(INITR_GREENTAB);   // initialize a ST7735S chip, green tab
  tft.setRotation(3);


  Serial.println("tft OK!");
  tft.fillScreen(ST7735_BLACK);

  text.initR(INITR_GREENTAB);   // initialize a ST7735S chip, green tab
  text.setRotation(3);
  Serial.println("OK!");
  text.fillScreen(ST7735_BLACK);

  

  // SD卡初始化
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card init error!"));
    text.setTextSize(2);
    text.fillScreen(ST7735_BLACK);
    text.setCursor(0, 0);
    text.setTextColor(ST7735_BLUE);
    text.print(F("SD Card init error!"));
  
  }
  else 
  {
    Serial.println(F("SD Card init success!"));
    text.setTextSize(2);
    text.fillScreen(ST7735_BLACK);
    text.setCursor(0, 0);
    text.setTextColor(ST7735_BLUE);
    text.print(F("SD Card init success!"));
  }



}

void loop() {
  //Serial.print(F("Initializing SD card..."));
  // SD卡初始化
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card init error!"));
    text.setTextSize(2);
    text.fillScreen(ST7735_BLACK);
    text.setCursor(0, 0);
    text.setTextColor(ST7735_BLUE);
    text.print(F("SD Card init error!"));
  
  }
  else 
  {
    Serial.println(F("SD Card init success!"));
    text.setTextSize(2);
    text.fillScreen(ST7735_BLACK);
    text.setCursor(0, 0);
    text.setTextColor(ST7735_BLUE);
    text.print(F("SD Card init success!"));
  }

  // TEXT
  text.fillScreen(ST7735_BLACK);  // 清屏

  // generate a random color
  int redRandom = random(0, 255);
  int greenRandom = random(0, 255);
  int blueRandom = random(0, 255);

  // set a random font color
  text.setTextColor(tft.color565(redRandom, greenRandom, blueRandom));

  // print in the middle of the screen
  text.setCursor(6, 57);
  text.setTextSize(2);
  text.print(F("Hey Barbie!"));

  // 检查串口是否有数据
  if (retft_Serial.available()) {
    char command[100];  // 假设最大长度为100
    retft_Serial.readBytesUntil('\n', command, sizeof(command));  // 读取串口命令
    Serial.println(F("get command"));
    Serial.println(command);

    // 每次打印前检查行数，超出屏幕底部时从"Hi, Barbie!"下方开始
    if (lineY + lineHeight > 160) {
      lineY = 57;  // 重置行坐标
      text.fillScreen(ST7735_BLACK);  // 清除屏幕背景（可选）
      text.setCursor(6, 57);
      text.print(F("Hey Barbie!"));  // 重新显示 "Hey Barbie!"
    }

    // 打印串口命令并更新行位置
    text.setCursor(6, lineY);
    text.print(command);
    lineY += lineHeight;  // 移动到下一行

    // 处理各种命令
    if (strcmp(command, "Choose Clothes") == 0) {
      int randomIndex = random(0, numClothes);  // 随机选择一张图片索引
      currentImage = clothes[randomIndex];
      bmpDraw(currentImage,0,0);  // 显示随机图片

      // 记录当前时间，用于控制图片显示持续时间
      imageDisplayMillis = millis();
    } else if (strcmp(command, "Display all clothes") == 0) {
      // 滚动显示所有服装图片
      for (int i = 0; i < numClothes; i++) {
        currentImage = clothes[i];
        bmpDraw(currentImage,0,0); // 显示当前图片
        displayAllMillis = millis();  // 记录当前时间

        // 等待 3 秒钟再显示下一张图片
        while (millis() - displayAllMillis < displayAllDuration) {
          // 空循环，等待 3 秒
        }
      }

      // 恢复显示 cover.bmp
      currentImage = "cover.bmp";
      bmpDraw(currentImage,0,0);
    }
    
  }
  Serial.println("cover");

  // 控制图片显示时间
  if (millis() - imageDisplayMillis >= imageDisplayDuration) {
    // 60秒后恢复显示 "cover.bmp"
    currentImage = "cover.bmp";
    bmpDraw(currentImage,0,0);
  }
}

void bmpDraw(char *filename, uint8_t x, uint8_t y) {
 
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
 
  if((x >= tft.width()) || (y >= tft.height())) return;
 
  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');
 
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }
  else {
    Serial.println("file loaded");
  }
 
  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
 
        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);
 
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
 
        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
 
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
 
        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, w, h);
 
        for (row=0; row<h; row++) { // For each scanline...
 
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?

            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }
 
          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
 
            // Convert pixel from BMP to TFT format, push to display
            r = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            b = sdbuffer[buffidx++];
            tft.drawPixel(x + col, y + row, tft.color565(r,g,b));
          } // end pixel
        } // end scanline

        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }
 
  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}
 
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
 
uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}
 
uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

