
#include <WS2812FX.h>

#define LED_COUNT 12
#define LED_PIN 12

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "TimeLib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

int g=0;
const char *ssid     = "Gia Huy";
const char *password = "1234567890";
String date_time;

WiFiClient wifiClient;
#define ON_Board_LED 2 
///////////////////////////////////////////////////////////////////////////////
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "3.ru.pool.ntp.org", 10800, 60000);// This definition is used for Russia, Zelenograd.
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String weekDays3Dgt[7]={"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
String months[12]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char* mist[] = { "Mist", "Smoke", "Haze","Dust", "Fog", "Sand","Dust","Ash","Squall","Tornado" };

String current_weather_Description;
String current_weather;
int current_temperature, current_pressure, current_humidity, current_wind_deg, current_visibility;
float current_wind_speed, current_feels_like, current_uv, current_dew_point;
String current_weather_sym;
static bool updateGet=0;
String forecast_weather[4];
String forecast_weather_sym[4];
float forecast_temp_min[4];
float forecast_temp_max[4];
///////////////////////////////////////////////////////////////////////////////
#define SCREEN_WIDTH 128 //--> OLED display width, in pixels
#define SCREEN_HEIGHT 64 //--> OLED display height, in pixels
//Special, my oled height is devided into 2 parts, 1 part yellow 128x16, 1 part blue 128x48
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/////////////////////////////////////////////////////////////////////////


//----------------------------------------openweathermap API configuration
String openWeatherMapApiKey = "295bd342bcc12dfe1d19710257e1bee0";

// Check subroutine "void Get_Weather_Data ()"
// If using a city name : String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + units + "&APPID=" + openWeatherMapApiKey;
// If using a city ID : String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?id=" + city + "&units=" + units + "&APPID=" + openWeatherMapApiKey;
// Please choose one.

//----------------------------------------
String city = "Zelenograd";
String countryCode = "RU";
String units = "metric";
//----------------------------------------Variable declaration for the json data and defining the ArduinoJson(DynamicJsonBuffer)
String strjsonBuffer;
String strjsonBufferCF;
DynamicJsonBuffer jsonBuffer;

//----------------------------------------

//----------------------------------------Variable declarations for weather and loading symbols / icons 
// 'Process1_Sym', 8x8px
const unsigned char Process1_Sym [] PROGMEM = {
  0x00, 0x18, 0x00, 0x01, 0x99, 0x80, 0x01, 0x81, 0x80, 0x08, 0x00, 0x10, 0x18, 0x00, 0x18, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x06, 
  0x60, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x08, 0x00, 0x10, 0x01, 
  0x81, 0x80, 0x01, 0x99, 0x80, 0x00, 0x18, 0x00
};


// 'Process2_Sym', 8x8px
const unsigned char Process2_Sym [] PROGMEM = {
  0x00, 0x66, 0x00, 0x00, 0x66, 0x00, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x0c, 0x30, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0x0c, 0x30, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x06, 
  0x00, 0x60, 0x00, 0x66, 0x00, 0x00, 0x66, 0x00
};

// 'Clear_Daylight_Sym', 24x24px
const unsigned char Clear_Daylight_Sym [] PROGMEM = {
  0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x10, 0x18, 0x08, 0x38, 0x00, 0x1c, 0x1c, 0x3c, 0x38, 0x08, 
  0xff, 0x10, 0x01, 0xc1, 0x80, 0x03, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 0x0c, 0x00, 
  0x30, 0xec, 0x00, 0x37, 0xec, 0x00, 0x37, 0x0c, 0x00, 0x30, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 
  0x03, 0x00, 0xc0, 0x01, 0xc3, 0x80, 0x08, 0xff, 0x10, 0x1c, 0x3c, 0x38, 0x38, 0x00, 0x1c, 0x10, 
  0x18, 0x08, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00
};

// 'Clear_Night_Sym', 24x24px
const unsigned char Clear_Night_Sym [] PROGMEM = {
  0x08, 0x08, 0x80, 0x08, 0x1c, 0xc0, 0x1c, 0x08, 0xe0, 0x3e, 0x00, 0xf0, 0xff, 0x80, 0xd8, 0x3e, 
  0x00, 0xcc, 0x1c, 0x00, 0xc6, 0x08, 0x01, 0x86, 0x08, 0x01, 0x83, 0x00, 0x03, 0x03, 0x00, 0x03, 
  0x03, 0x40, 0x06, 0x03, 0xe0, 0x06, 0x03, 0x40, 0x1c, 0x03, 0x00, 0x78, 0x03, 0x01, 0xe0, 0x03, 
  0xff, 0x80, 0x06, 0x7e, 0x00, 0x06, 0x30, 0x00, 0x0c, 0x18, 0x00, 0x18, 0x0c, 0x00, 0x30, 0x07, 
  0x00, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0xff, 0x00
};

// 'Thunderstorm', 24x24px
const unsigned char Thunderstorm_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0xf8, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60, 
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff, 
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x10, 0x1e, 0x02, 0x20, 0x24, 0x04, 0x40, 0x48, 0x08, 
  0xf8, 0x9f, 0x9f, 0x11, 0x00, 0x82, 0x21, 0xfd, 0x04, 0x40, 0x0a, 0x08, 0x80, 0x14, 0x10, 0x00, 
  0x28, 0x00, 0x00, 0x50, 0x00, 0x00, 0x60, 0x00
};

// 'Drizzle', 24x24px
const unsigned char Drizzle_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0x18, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60, 
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff, 
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc3, 0x0c, 0x30, 0xc3, 0x0c, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x18, 0x61, 0x86, 0x18, 0x61, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x30, 0xc3, 0x0c, 0x30, 0xc3, 0x0c
};

// 'Rain', 24x24px
const unsigned char Rain_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0xf8, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60,
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff, 
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x21, 0x21, 0x08, 0x42, 
  0x42, 0x10, 0x84, 0x84, 0x21, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x21, 0x21, 
  0x08, 0x42, 0x42, 0x10, 0x84, 0x84, 0x21, 0x08
};

// 'Snow', 24x24px
const unsigned char Snow_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0x18, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60, 
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff, 
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0xa0, 0x50, 0x20, 0x40, 0x20, 0xa8, 0xa0, 0x50, 0x70, 
  0x00, 0x01, 0xfc, 0x04, 0x00, 0x70, 0x15, 0x00, 0xa8, 0x0e, 0x00, 0x20, 0x3f, 0x80, 0x00, 0x0e, 
  0x0a, 0x05, 0x15, 0x04, 0x02, 0x04, 0x0a, 0x05
};

// 'Mist', 24x24px
const unsigned char Mist_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0x18, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60, 
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff, 
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc3, 0x0c, 0x79, 0xe7, 0x9e, 
  0xcf, 0x3c, 0xf3, 0x86, 0x18, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc3, 0x0c, 0x79, 
  0xe7, 0x9e, 0xcf, 0x3c, 0xf3, 0x86, 0x18, 0x61
};

// 'Clouds_Daylight', 24x24px
const unsigned char Clouds_Daylight_Sym [] PROGMEM = {
  0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x10, 0x18, 0x08, 0x38, 0x00, 0x1c, 0x1c, 0x3c, 0x38, 0x08, 
  0xff, 0x10, 0x01, 0xc3, 0x80, 0x03, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 0x0c, 0x00, 
  0x30, 0xec, 0x00, 0x37, 0xed, 0xf0, 0x37, 0x0f, 0xf8, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 
  0x38, 0x03, 0x06, 0x60, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 
  0x00, 0x07, 0x7f, 0xff, 0xfe, 0x3f, 0xff, 0xfc
};

// 'Clouds_Night_Sym', 24x24px
const unsigned char Clouds_Night_Sym [] PROGMEM = {
  0x00, 0x00, 0x02, 0x00, 0x01, 0x07, 0x04, 0x01, 0xc2, 0x04, 0x01, 0xe0, 0x0e, 0x01, 0xb0, 0x3f, 
  0x81, 0x98, 0x0e, 0x01, 0x8c, 0x04, 0x03, 0x0c, 0x04, 0x03, 0x06, 0x00, 0x03, 0x06, 0x00, 0x06, 
  0x03, 0x00, 0x06, 0x03, 0x00, 0x0c, 0x03, 0x00, 0x38, 0x03, 0x01, 0xf0, 0x06, 0x7f, 0xc0, 0x06, 
  0x3e, 0x00, 0x0c, 0x30, 0x00, 0x0c, 0x18, 0x00, 0x18, 0x0c, 0x00, 0x30, 0x07, 0x00, 0xe0, 0x43, 
  0xc3, 0xc2, 0xe0, 0xff, 0x07, 0x40, 0x3c, 0x02
};

// 'Cloudy_Sym', 24x24px
const unsigned char Cloudy_Sym [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x01, 0xf0, 0xe0, 0x03, 0x19, 0xf0, 0x0e, 
  0x0f, 0x1c, 0x1c, 0x06, 0x0e, 0x30, 0x00, 0x03, 0x60, 0x00, 0x03, 0x61, 0xc0, 0x03, 0x63, 0xe1, 
  0xc3, 0x36, 0x33, 0xe6, 0x3c, 0x1e, 0x3c, 0x78, 0x0c, 0x1c, 0xe0, 0x00, 0x06, 0xc0, 0x00, 0x06, 
  0xc0, 0x00, 0x06, 0xc0, 0x00, 0x06, 0xe0, 0x00, 0x0c, 0x7f, 0xff, 0xf8, 0x3f, 0xff, 0xf0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



int cnt_con = 0;//time retry to connect
void Connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    ws2812fx.setSpeed(300);
     ws2812fx.service();
     ws2812fx.setMode(g);
     if(g==54)g=0;
        g++;
    Serial.print(".");
    //----------------------------------------Displays the text and icon of the connection process on OLED
    digitalWrite(ON_Board_LED, LOW); //--> Make the On Board Flashing LED on the process of connecting to the wifi router.

    display.clearDisplay(); //--> for Clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 1);
    display.println("Connecting");
    display.drawBitmap(52, 25, Process1_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    //display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
    display.display();
    delay(50);
    display.clearDisplay(); //--> for Clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 1);
    display.println("Connecting.");
    display.drawBitmap(52, 25, Process2_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    display.display();
    delay(150);
    digitalWrite(ON_Board_LED, HIGH); //--> Make the Off Board Flashing LED on the process of connecting to the wifi router.
    
    display.clearDisplay(); //--> for Clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 1);
    display.println("Connecting..");  
    display.drawBitmap(52, 25, Process1_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    //display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
    display.display();
    delay(150);
    display.clearDisplay(); //--> for Clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 1);
    display.println("Connecting...");
    display.drawBitmap(52, 25, Process2_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    display.display();
    delay(150);
    cnt_con++;
    if(cnt_con > 59) {
      cnt_con = 0;
      Serial.println("Failed to connect !");
      display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 28);
      display.println(" Failed to connect !");
      //display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
      display.display();
      //delay(500);
      Waiting_to_be_connected_again();
    }
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  display.clearDisplay(); //--> for Clearing the display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 26);
  display.println("     Connection");
  display.setCursor(0, 35);
  display.println("     Successful!");
  //display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  display.display();
  delay(500);
  //----------------------------------------
}

void Connection_lost() {
  Serial.println("WiFi Disconnected");
  display.clearDisplay(); //--> for Clearing the display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 28);
  display.println("  Connection lost !");
  //display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  display.display();
  delay(50);
  Connect();
}
//=====================================================================================

//=====================================================================================Subroutines waiting to be reconnected
void Waiting_to_be_connected_again() {
  for(int i = 30; i > -1; i--) {
    ws2812fx.service();
    Serial.print("Will be connected again:");
    Serial.println(i);
    display.clearDisplay(); //--> for Clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);
    display.println("       Will be");
    display.setCursor(0, 33);
    display.print("  connected again:");
    display.println(i);
    //display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
    display.display();
    delay(50);
  }
  Connect();
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  delay(200);
  ws2812fx.init();
  ws2812fx.setBrightness(200);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x00FFFF);
  //ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();

  //----------------------------------------SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // Address 0x3C for 128x32 and Address 0x3D for 128x64.
  // But on my 128x64 module the 0x3D address doesn't work. What works is the 0x3C address.
  // So please try which address works on your module.
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //--> Don't proceed, loop forever
  }
  //----------------------------------------
  
  //----------------------------------------Show initial display buffer contents on the screen
  // the library initializes this with an Adafruit splash screen.
  // Show the display buffer on the screen(Update screen). You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  Connect();
  timeClient.begin();
  GetWeather();
  
  
}
////////////////////////////////////////////////////////////////////////////////////////////////












void loop() {
   ws2812fx.setMode(g);
   ws2812fx.setSpeed(1000);
  if(WiFi.status()== WL_CONNECTED){
    
      long b =millis();
      String a = timeClient.getFormattedTime();
      if(updateGet==0&&a[4]=='0'){
          GetWeather();
           updateGet=1;
           
      }if(a[4]!='0'){
        updateGet=0;
       
      }
      while(1){
      ws2812fx.service();
      
      if(millis()-b<5000&&millis()-b>0){
      
      Display_current_weather_data();
      }else if(millis()-b>5000&&millis()-b<10000){
       
      Display_weather_forecast_data();
      
      }else if(millis()-b>10000) {
        if(g==54)g=0;
        g++;
        break;
      }
      }
      















    
  } else {
     
    Connection_lost();
  }
}
void Display_time(){
      timeClient.update();
      date_time = timeClient.getFormattedDate();
      int index_date = date_time.indexOf("T");
      String date = date_time.substring(0, index_date);
      String nightlight = timeClient.getFormattedTime();
      display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      display.drawRect(0, 0, 128, 16, WHITE); //--> Display frame
      display.setCursor(4,5); //--> (x position, y position)
      display.println(date);
      display.setCursor(70,5 );
      display.println(weekDays[timeClient.getDay()]);
      //Serial.println(timeClient.getFormattedTime());
      if((nightlight[0]-'0'>1&&nightlight[1]-'0'>2)||(nightlight[0]-'0'==0&&nightlight[1]-'0'<9)){
        ws2812fx.setBrightness(50);
        ws2812fx.setColor(0xFFFF00);
      }
      else{
         ws2812fx.setBrightness(50);
        ws2812fx.setColor(0x00FFFF);
      }
      
      //display.println("Fail roi ah");
}
void GetWeather(){
  if(WiFi.status()== WL_CONNECTED){
    
    timeClient.update();
         //----------------------------------------If using a city name
    String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + units + "&APPID=" + openWeatherMapApiKey;


    strjsonBuffer = httpGETRequest(current_serverPath.c_str());
    //Serial.println(strjsonBuffer);
    JsonObject& root = jsonBuffer.parseObject(strjsonBuffer);
  


       display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);      
      display.setCursor(10 ,20); //--> (x position, y position)
      display.println("Getting weather data...");

      // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 19);
      display.println("    Failed to get");
      display.setCursor(0, 28);
      display.println("    weather data !");
      display.setCursor(4, 37);
      display.println("parseObject() failed");
      display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
      display.display();
      delay(5000);
      return;
    }
    String lon = root["coord"]["lon"];
    String lat = root["coord"]["lat"];
    jsonBuffer.clear();

    String current_forecast_serverPath = "http://api.openweathermap.org/data/2.5/onecall?lat=" + lat + "&lon=" + lon + "&units=" + units + "&exclude=hourly&APPID=" + openWeatherMapApiKey;

    strjsonBufferCF = httpGETRequest(current_forecast_serverPath.c_str());
    //Serial.println(strjsonBuffer);
    JsonObject& rootCF = jsonBuffer.parseObject(strjsonBufferCF);
    if (!rootCF.success()) {
      Serial.println("parseObject() failed");
      display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 19);
      display.println("    Failed to get");
      display.setCursor(0, 28);
      display.println("    weather data !");
      display.setCursor(4, 37);
      display.println("parseObject() failed");
      display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
      display.display();
      delay(5000);
      return;
    }

    String str_current_weather = rootCF["current"]["weather"][0]["main"];
    current_weather = str_current_weather;
    
    String str_current_weather_Description = rootCF["current"]["weather"][0]["description"];
    current_weather_Description = str_current_weather_Description;

    String str_current_weather_sym = rootCF["current"]["weather"][0]["icon"];
    current_weather_sym = str_current_weather_sym;
  
    current_temperature = rootCF["current"]["temp"];
    current_feels_like = rootCF["current"]["feels_like"];
    current_uv = rootCF["current"]["uvi"];
    current_dew_point = rootCF["current"]["dew_point"];
    current_pressure = rootCF["current"]["pressure"];
    current_humidity = rootCF["current"]["humidity"];
    current_visibility = rootCF["current"]["visibility"];
    current_wind_speed = rootCF["current"]["wind_speed"];
    current_wind_deg = rootCF["current"]["wind_deg"];
    //----------------------------------------

    //----------------------------------------Get weather forecast data for the following days
    String str_forecast_weather1 = rootCF["daily"][0]["weather"][0]["main"];
    forecast_weather[0] = str_forecast_weather1;
    String str_forecast_weather_sym1 = rootCF["daily"][0]["weather"][0]["icon"];
    forecast_weather_sym[0] = str_forecast_weather_sym1;
    forecast_temp_max[0] = rootCF["daily"][0]["temp"]["max"]; 
    forecast_temp_min[0] = rootCF["daily"][0]["temp"]["min"]; 
    
    String str_forecast_weather2 = rootCF["daily"][1]["weather"][0]["main"];
    forecast_weather[1] = str_forecast_weather2;
    String str_forecast_weather_sym2 = rootCF["daily"][1]["weather"][0]["icon"];
    forecast_weather_sym[1] = str_forecast_weather_sym2;
    forecast_temp_max[1] = rootCF["daily"][1]["temp"]["max"]; 
    forecast_temp_min[1] = rootCF["daily"][1]["temp"]["min"];

    String str_forecast_weather3 = rootCF["daily"][2]["weather"][0]["main"];
    forecast_weather[2] = str_forecast_weather3;
    String str_forecast_weather_sym3 = rootCF["daily"][2]["weather"][0]["icon"];
    forecast_weather_sym[2] = str_forecast_weather_sym3;
    forecast_temp_max[2] = rootCF["daily"][2]["temp"]["max"]; 
    forecast_temp_min[2] = rootCF["daily"][2]["temp"]["min"];

    String str_forecast_weather4 = rootCF["daily"][3]["weather"][0]["main"];
    forecast_weather[3] = str_forecast_weather4;
    String str_forecast_weather_sym4 = rootCF["daily"][3]["weather"][0]["icon"];
    forecast_weather_sym[3] = str_forecast_weather_sym4;
    forecast_temp_max[3] = rootCF["daily"][3]["temp"]["max"]; 
    forecast_temp_min[3] = rootCF["daily"][3]["temp"]["min"];
    //----------------------------------------


    jsonBuffer.clear();
    Serial.println("Get succesful");
  
     
  
  }
  else {
    display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      
      display.setCursor(10 ,20); //--> (x position, y position)
      display.println("WiFi Disconnected");
    
  }
}
String httpGETRequest(const char* serverName) {
    HTTPClient http;

    // Your IP address with path or Domain name with URL path 
    http.begin(wifiClient, serverName);

    // Send HTTP POST request
    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode == 200) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);

        display.clearDisplay(); //--> for Clearing the display
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 19);
        display.println("    Failed to get");
        display.setCursor(0, 28);
        display.println("    weather data !");
        display.setCursor(0, 37);
        display.print("  HTTP Response: ");
        display.println(httpResponseCode);
        display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
        display.display();
        delay(5000);
    }
    // Free resources
    http.end();

    return payload;
}

//=====================================================================================

//=====================================================================================Subroutines for displaying weather data
void Display_current_weather_data() {
  display.clearDisplay();
  Display_time();
  //----------------------------------------Weather icon selection conditions
  if (current_weather == "Thunderstorm") {
    display.drawBitmap(10, 15+10, Thunderstorm_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Drizzle") {
    display.drawBitmap(10, 15+10, Drizzle_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Rain") {
    display.drawBitmap(10, 15+10, Rain_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Snow") {
    display.drawBitmap(10, 15+10, Snow_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Clear") {
    if (current_weather_sym == "01d") {
      display.drawBitmap(10, 5+15+10, Clear_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    } else if (current_weather_sym == "01n") {
      display.drawBitmap(10, 15+5+10, Clear_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);  
    }
  }
  else if (current_weather == "Clouds") {
    if (current_weather_sym == "02d") {
      display.drawBitmap(10, 5+15+10, Clouds_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    } else if (current_weather_sym == "02n") {
      display.drawBitmap(10, 15+10, Clouds_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    } else {
      display.drawBitmap(10, 15+10, Cloudy_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
  }

  for (int i = 0; i < 10; i++) {
    if (current_weather == mist[i]) {
      display.drawBitmap(10, 5+15+10, Mist_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
  }
  //----------------------------------------

  //----------------------------------------Displays weather data, temperature, humidity and wind speed
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  //display.setCursor(50, 5+15); //--> (x position, y position)
  //display.println(current_weather);

  //display.setCursor(50, 15+15); //--> (x position, y position)
  //String Hum = "H:" + String(current_humidity) + "%";
  //display.println(Hum);

  //display.setCursor(50, 25+15); //--> (x position, y position)
  //String WS = "WS:" + String(current_wind_speed) + "m/s";
  //display.println(WS);


  display.setCursor(60, 10+15); //--> (x position, y position)
 
  display.print(current_temperature);
  display.print((char)247); //--> ASCII degree symbol
  display.print("C");
  display.setCursor(60, 10+15+20); //--> (x position, y position)
  String Hum = String(current_humidity) + "%";
  display.println(Hum);
  //----------------------------------------
  display.display();
}
//=====================================================================================

//=====================================================================================Subroutines to display data for the next few days
void Display_weather_forecast_data() {
  display.clearDisplay();
  Display_time();
  long a=millis();
  int forecast_X1[] = {20,90};
  int forecast_X2[] = {17,87};
  
  int wds[4] = {timeClient.getDay(),timeClient.getDay()+1,timeClient.getDay()+2,timeClient.getDay()+3};


  for(int i = 0; i < 4; i++) {
    if(wds[i] == 7) {
      wds[i] = 0;
    }
    if(wds[i] == 8) {
      wds[i] = 1;
    }
    if(wds[i] == 9) {
      wds[i] = 2;
    }
  }

  String forecast_weather_Prcs[2];
  String forecast_weather_sym_Prcs[2];
  int wds_Prcs[2];



    
    forecast_weather_Prcs[0] = forecast_weather[1];
    forecast_weather_Prcs[1] = forecast_weather[2];
    forecast_weather_sym_Prcs[0] = forecast_weather_sym[1];
    forecast_weather_sym_Prcs[1] = forecast_weather_sym[2];
    wds_Prcs[0] = wds[0];
    wds_Prcs[1] = wds[1];
   
  
    
 


  String wd;
  String WF, WSF;

  
  display.setTextSize(1);
  display.setTextColor(WHITE);

  //----------------------------------------Process data for display
  for(int i = 0; i < 2; i++) {
    wd = weekDays3Dgt[wds[i]+1];
    display.setCursor(forecast_X1[i], 18);
    display.print(wd);
    display.setCursor(forecast_X1[i]-4, 56);
    display.print((int)(forecast_temp_max[i+1]));
    display.print((char)247); //--> ASCII degree symbol
    display.print("C");

    WF = forecast_weather_Prcs[i];
    WSF = forecast_weather_sym_Prcs[i];
    //Serial.println(wd);
    
    if (WF == "Thunderstorm") {
      display.drawBitmap(forecast_X2[i], 9+18, Thunderstorm_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Drizzle") {
      display.drawBitmap(forecast_X2[i], 9+18, Drizzle_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Rain") {
      display.drawBitmap(forecast_X2[i], 9+18, Rain_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Snow") {
      display.drawBitmap(forecast_X2[i], 9+18, Snow_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Clear") {
      if (WSF == "01d") {
        display.drawBitmap(forecast_X2[i], 9+18, Clear_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      } else if (WSF == "01n") {
        display.drawBitmap(forecast_X2[i], 9+18, Clear_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);  
      }
    }
    else if (WF == "Clouds") {
      if (WSF == "02d") {
        display.drawBitmap(forecast_X2[i], 9+18, Clouds_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      } else if (WSF == "02d") {
        display.drawBitmap(forecast_X2[i], 9+18, Clouds_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      } else {
        display.drawBitmap(forecast_X2[i], 9+18, Cloudy_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      }
    }
    
    for (int i = 0; i < 10; i++) {
      if (WF == mist[i]) {
        display.drawBitmap(forecast_X2[i], 9+18, Mist_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      }
    }

    

  }
  //----------------------------------------

  

  
  
  display.display();
}
