#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// #define VBAND_PIN 32

extern TFT_eSPI tft; 
extern XPT2046_Touchscreen ts;
extern String inputvolts();
extern 



void calibrate_touch() 
{
  long x1=0,y1=0,count=0; 
  int vv;

  tft.fillScreen(TFT_BLACK);
 
  tft.drawRect(10,10,460,300,TFT_RED); // Calibrate 480 x 320
  tft.drawCircle(10,10,5,TFT_RED);


  while(1) 
  {
   
	  ulong starttime = millis();
    while(ts.touched())
    {
      	if(millis()-starttime > 300)
        {
          TS_Point p = ts.getPoint(); 
          count++;
          x1=x1+p.x;
          y1=y1+p.y;

          Serial.print("x=  ");Serial.print(p.x);Serial.print(", x_avg=  ");Serial.print(x1/count);
          Serial.print(", y=  ");Serial.print(p.y);Serial.print(", y_avg=  ");Serial.println(y1/count);
          break;
        }
    }
       
  // vv=inputvolts();
  
  }

  return;
}


