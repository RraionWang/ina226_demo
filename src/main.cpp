#include "Arduino.h"
#include "Wire.h"
#include "INA226.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1

// 使用 Wire1 给 OLED 独立 I²C
TwoWire I2C_OLED = TwoWire(1);  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET);
INA226 INA(0x40);   // INA226 默认地址 0x40

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("INA226_LIB_VERSION: ");
  Serial.println(INA226_LIB_VERSION);

  // INA226: I²C0
  Wire.begin(5, 6);   // SDA=GPIO5, SCL=GPIO6
  if (!INA.begin())
  {
    Serial.println("INA226 could not connect. Fix wiring!");
    while (1);
  }
 
  
  // INA.setMaxCurrentShunt(0.7, 0.1);

  // 设置量程（你已正确使用）
INA.setMaxCurrentShunt(0.7, 0.1);

// ✅ 正确设置平均采样、转换时间和模式
INA.setAverage(INA226_16_SAMPLES);                    // 16 次平均
INA.setBusVoltageConversionTime(INA226_1100_us);      // 总线电压转换时间：1.1ms
INA.setShuntVoltageConversionTime(INA226_1100_us);    // 分流电压转换时间：1.1ms
INA.setModeShuntBusContinuous();                      // 连续测量总线和分流电压

  


  // OLED: I²C1
  I2C_OLED.begin(7, 8);   // SDA=GPIO7, SCL=GPIO8
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("INA226 + OLED OK");
  display.display();
  delay(2000);
}

void loop()
{
  float busV   = INA.getBusVoltage();
  float shuntV = INA.getShuntVoltage_mV();
  float current= INA.getCurrent_mA();
  float power  = INA.getPower_mW();

  // 串口输出
  Serial.print("Bus[V]: "); Serial.print(busV,3);
  Serial.print("\tShunt[mV]: "); Serial.print(shuntV,3);
  Serial.print("\tCurrent[mA]: "); Serial.print(current,3);
  Serial.print("\tPower[mW]: "); Serial.println(power,3);

  // OLED 输出
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("INA226 Monitor");

  display.setCursor(0,16);
  display.print("Bus: ");
  display.print(busV,3); display.println(" V");

  display.setCursor(0,28);
  display.print("Shunt: ");
  display.print(shuntV,3); display.println(" mV");

  display.setCursor(0,40);
  display.print("Current: ");
  display.print(current,1); display.println(" mA");

  display.setCursor(0,52);
  display.print("Power: ");
  display.print(power,1); display.println(" mW");

  display.display();

  delay(1000);
}
