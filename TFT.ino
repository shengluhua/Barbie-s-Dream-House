 
 
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>
 
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif
 
// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#define SD_CS    4  // Chip select line for SD card
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   9  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)


 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// 使用 C 风格字符串替代 String
const char* clothes[] = {
  "black.bmp",
  "blue.bmp",
  "chistmas.bmp",
  "fish.bmp",
  "green.bmp",
  "pink.bmp",
  "purple.bmp",
  "school.bmp",
  "work.bmp"
};

const int numClothes = sizeof(clothes) / sizeof(clothes[0]);  // 图片总数
const int displayTime = 3 * 1000;  // 图片显示时间（3秒，单位为毫秒）

const char* currentImage = "cover.bmp";  // 当前显示的图片

unsigned long previousMillis = 0;
unsigned long displayAllMillis = 0;      // 用于控制滚动显示服装的计时


 
#define BUFFPIXEL 20
 
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
 
void setup(void) {
  pinMode(12,INPUT); // Set SD's MISO IO State, VERY IMPORTANT!
  Serial.begin(9600);
 
  // Initialize 1.8" TFT
  tft.initR(INITR_GREENTAB);   // initialize a ST7735S chip, green tab
  tft.setRotation(3);
 
  Serial.println("OK!");
  tft.fillScreen(ST7735_BLACK);


}
 
void loop() {
    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
      Serial.println("failed!");
      tft.setTextSize(2);
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ST7735_BLUE);
      tft.print("SD Card init error!");
      return;
    }
       bmpDraw("cover.bmp",0,0);
       while (millis() - displayAllMillis < 10000) {
          // 空循环，等待 10 秒
        }

// 滚动显示所有服装图片
      for (int i = 0; i < numClothes; i++) {
        currentImage = clothes[i];
        bmpDraw(currentImage,0,0); // 显示当前图片
        displayAllMillis = millis();  // 记录当前时间

        // 等待 3 秒钟再显示下一张图片
        while (millis() - displayAllMillis < displayTime) {
          // 空循环，等待 3 秒
        }
      }
       bmpDraw("cover.bmp",0,0);
       while (millis() - displayAllMillis < 10000) {
          // 空循环，等待 10 秒
        }

      int randomIndex = random(0, numClothes);  // 随机选择一张图片索引
      currentImage = clothes[randomIndex];
      bmpDraw(currentImage,0,0);  // 显示随机图片
       while (millis() - displayAllMillis < 50000) {
          // 空循环，等待 10 秒
        }



}