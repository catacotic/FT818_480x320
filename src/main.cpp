
#include "globals.h"
#include "main.h"

extern bool first_run;
String OLD_VOLTS="          ";
String BAT_VOLTS="          ";  // variable for storing the bat volts (string)
int BAT_STEPS = 0;              // variable for storing the bat volts (graphic indicator)
int OLD_STEPS = 0;

bool bat_refresh=false;         // indicate if bat or usb valors changed
bool VCHAR, OLD_VCHAR;          // variable for storing the power USB status
bool OLD_PWON;




// ---------------------------------------------
//      Setup
// ---------------------------------------------
void setup()
{
 Serial.begin(9600); 
 radio.begin(38400,SERIAL_8N1,16,17);
 POWER_ON = false; // defined in main.h
 OLD_PWON = false;

 pinMode(PTUNE_PIN, OUTPUT );
 analogSetPinAttenuation(POF_PIN, ADC_11db ); // read FT818 band analog tension for POWER (on/off) status
 analogSetPinAttenuation(PBAT_PIN, ADC_11db ); // read BAT tension for bat volts control
 analogSetPinAttenuation(VUSB_PIN, ADC_11db ); // read USB tension for CHARGE on/off status

 start_power(); // Init POWER_ON, BAT VOLTS & CHARGE ON/OFF

 // Initialize the touch library
 tft.init();
 tft.setRotation(3);
 ts.setRotation(1);
 ts.begin();
 
 // call screen calibration
 // calibrate_touch();

  // Clear the scree1n
  tft.fillScreen(TFT_BLACK);
  show_credits();
  // Init the status field lines - 480x320:

  field[0].initField( 30, 146,   8, 142, 55, 20);
  field[1].initField(97, 146,  75, 142, 55, 20);
  field[2].initField(165, 146, 143, 142, 55, 20);
  field[3].initField(232, 146, 210, 142, 55, 20);
  field[4].initField(300, 146, 278, 142, 55, 20);
  field[5].initField(365, 146, 345, 142, 55, 20);
  field[6].initField(435, 146, 413, 142, 55, 20);

  // fields below the frequency display:
  field[7].initField( 30, 241,    8, 237, 55, 20);
  field[8].initField( 97, 241,  75, 237, 55, 20);
  field[9].initField( 165, 241, 143, 237, 55, 20);
  field[10].initField(232, 241, 210, 237, 55, 20);
  field[11].initField(300, 241, 278, 237, 55, 20);
  field[12].initField(365, 241, 345, 237, 0, 20);
  field[13].initField(443, 241, 413, 237, 0, 20);

  
  refresh_required = true;  // ensure that frequency display is initially shown

  drawKeypad();
  drawSMeter();
  text_show_bar();
  init_all();
 


}
String inputbatvolts(){

 
 float vin;
 uint16_t batvolts = analogRead(PBAT_PIN);
 vin = batvolts * 0.00134;
 String rvolts =  String(vin, 1);
 BAT_STEPS = (int) ((vin-3.1)*90)/20+1;
 return(rvolts);
  

}

boolean inputchrvolts(){
  uint16_t chrvolts; 
  chrvolts = analogRead(VUSB_PIN);
  if (chrvolts > 1500) {return (true);} else {return(false);}
   
 }

bool inputFTpw(){

  int16_t pwvalue =analogRead(POF_PIN);
  if (pwvalue>70){
    return(true);
  }
 else
    return(false);   
 }

void start_power()
{
   // Analog PW mode
  POWER_ON = inputFTpw(); 
  OLD_PWON = !POWER_ON;
  VCHAR = inputchrvolts();
  BAT_VOLTS = inputbatvolts();
  bat_refresh=true; 
 
}

void read_SYSpw()

{
   // Analog PW mode
  
  VCHAR = inputchrvolts();
  BAT_VOLTS = inputbatvolts();
  if ((BAT_VOLTS != OLD_VOLTS) || (BAT_STEPS !=OLD_STEPS) || (VCHAR != OLD_VCHAR)){
     OLD_VOLTS = BAT_VOLTS;
     OLD_STEPS = BAT_STEPS;
     OLD_VCHAR = VCHAR;
     bat_refresh=true;
  }
  
}

void show_SYSpw(){
  
  int x2=278;
  static ulong refreshtime; 
  int y2=237;
  int x1=344;
  int y1=242;
  int bcolor;
  int arrowcolor = TFT_YELLOW; 
  char csvolts[5];
  //  field[11].initField(300, 241, 278, 237, 55, 20);
  if ((bat_refresh) && (millis() > (refreshtime + 1000)) ){
      BAT_VOLTS.toCharArray(csvolts, BAT_VOLTS.length()+1);
      if (VCHAR){
      tft.fillRect(x2, y2, 55, 20, TFT_DARKBLUE);
      tft.drawLine(x2+25, y2+4, x2+21, y2+16, arrowcolor);
      tft.drawLine(x2+26, y2+4, x2+22, y2+16, arrowcolor);
      tft.drawLine(x2+27, y2+4, x2+23, y2+16, arrowcolor);
      tft.drawLine(x2+28, y2+4, x2+24, y2+16, arrowcolor);
      tft.drawLine(x2+29, y2+4, x2+25, y2+16, arrowcolor);
      tft.fillTriangle(x2+4,y2+10, x2+25,y2+4,x2+23,y2+9,arrowcolor);
      tft.fillTriangle(x2+45,y2+10, x2+27,y2+16,x2+21,y2+11,arrowcolor);
      }  
    else{
      field[11].drawField(csvolts, TFT_YELLOW, TFT_DARKBLUE);
    }
    tft.fillRect(x1-2,y1-2,50, 17, TFT_DARKBLUE);
    tft.drawRect(x1-2,y1-2,50,17,TFT_SILVER);
    tft.drawRect(x1-3,y1-3,52,19,TFT_SILVER);
    if (BAT_STEPS < 0 ) {BAT_STEPS=0;}
    if (BAT_STEPS >= 0 ) {bcolor=TFT_DARKBLUE;}
    if (BAT_STEPS >= 1 ) {bcolor=TFT_RED;}
    if (BAT_STEPS >= 2 ) {bcolor=TFT_ORANGE;}
    if (BAT_STEPS >= 4 ) {bcolor=TFT_GREEN;}
    if (BAT_STEPS > 5 ) {BAT_STEPS=5;}
    for(uint8_t i=0; i<BAT_STEPS; i++) {
      tft.fillRect(x1+(i*10), y1, 8, 13, bcolor);
    }  
    bat_refresh=false;
    refreshtime=millis();
  }                 
}

void show_FTpw()
{
     tft.drawRect(418,226,26,26,TFT_SILVER);
     tft.drawRect(419,227,24,24,TFT_SILVER);
     if (POWER_ON){
     tft.fillRect(422,230,18,18,TFT_GREEN);
        field[7].drawField(pwr[current_pwr], TFT_YELLOW, TFT_DARKBLUE);
        field[5].drawField(agc[current_agc], TFT_YELLOW, TFT_DARKBLUE);
     }
     else{
        plotNeedle(0, 0);  // put Smeter -> 0 db
          tft.fillRect(65, 241, 70, 18, TFT_DARKBLUE);   
          for(uint8_t i=0; i<11; i++) {
               field[i].drawField("n/c", TFT_YELLOW, TFT_DARKBLUE); // put fileds n/c (no connect)
          }                       
          tft.fillRect(290,16,180,100,TFT_BLACK); // clear bars
          tft.fillRect(4,170,468,42,TFT_BLACK); // 480 x 320 // clear lock, band and freq
          tft.fillRect(422,230,18,18,TFT_RED); // 480 x 320

     }

} 
void control_power()
{  
  
   // read_SYSpw();
   // show_SYSpw();
    POWER_ON = inputFTpw(); 
    if ((POWER_ON != OLD_PWON) ){
      if (POWER_ON){
        init_all();
        show_FTpw();
        read_SYSpw();
        show_SYSpw();
        OLD_PWON = POWER_ON;
        }
      else {
          release_keys();
          show_FTpw();
          while (!POWER_ON) 
          {
            POWER_ON = inputFTpw(); 
            read_SYSpw();
            show_SYSpw();
          }
       OLD_PWON = !POWER_ON;
      }
    }  
}




// --------------------------------
//         main loop
// ---------------------------------
void loop()
{
  
  // proves en loop
  long x1=0,y1=0,count=0;
 
   
   ulong starttime = millis();

   long temp_f = f;

    // mqtt
  
   
control_power();
   show_freq();
// control_power();
   uint8_t b = check_menu_buttons();
   button_events(b);
control_power();
    txStatus = radio.chkTX();
    if(txStatus == 0xFF) {      // RX
      // Update S-Meter
      byte sm = radio.getSMeter();
        if(sm != old_sm) {
        plotNeedle(sm, 0);
       char buf[5];
       sprintf(buf,"%d",((sm*50)/14));
       send_mqtt("smeter", buf);
       old_sm = sm;
      }
      if( old_txStatus != txStatus)   // switched back from TX to RX. Restore S-Meter once.
      drawSMeter();
    }
    else {
      char timestr[10];
      tft.setTextDatum(TL_DATUM);
      tft.setTextColor(TFT_SILVER,TFT_DARKRED);
      if( old_txStatus != txStatus) {     // just starting TX
        start_tx_time = millis();
         // 480 x 320
        tft.fillRect(5, 6, 200, 107, TFT_BLACK);
        tft.drawRect(60,35,45,40,TFT_SILVER);
        tft.drawRect(15, 16, 197, 92,TFT_SILVER);
        tft.fillRect(16, 17, 194, 89,TFT_DARKRED);
        tft.drawString("TX",103,36,4); 
      }
     uint32_t tx_time = millis() - start_tx_time;
     sec2time(timestr,tx_time/1000);
     tft.drawString(timestr,87,71,4); // 480x320
    }
    old_txStatus = txStatus;
   
// control_power(); 
    uint16_t tx_meter = radio.read_TX_Meter();    // analog values 0 ... 10
    if( tx_meter == 0xFFFF) 
      tx_meter = 0;           // RX
  
    show_bar((tx_meter & 0x00F0) >> 4,0);  // power meter
    show_bar((tx_meter & 0xF000) >> 12,1); // swr meter
    show_bar((tx_meter & 0x000F),2);       //mod meter
    show_bar((tx_meter & 0x0F00) >> 8,3);  // alc meter
control_power(); 
    get_mode();     // read curent mode from FT817/818
control_power(); 
    // the following code is called only each one second
    // to reduce frequency of EEPROM reads
   if ( (millis() - updateTime) > 1000) {
        get_steps();  control_power();    // get configured steps for ssb, am and fm
        get_vfo();    control_power();  
        get_band();   control_power();  
        get_ipo();    control_power();       // get IPO status of the actual channel
        get_att();    control_power();       // get ATT status of the actual channel 
        get_nb();     control_power();       // get NB status of the actual channel
        get_antenna();   control_power();    // get assignment of front/rear antenna plugs to bands  
        get_pwr_sets();  control_power();  
        get_agc();     control_power();  
        get_chg();     control_power();  
        is_charging(); control_power();  
        get_nb_agc_lock(); control_power(); 
        updateTime = millis(); 
   }
  read_SYSpw();
  show_SYSpw(); 
  
}



void push_tunner()
{
  digitalWrite(PTUNE_PIN, HIGH); // Set signal on to active ATU 100 Tunner
  delay(400);                     // waits for a second
  digitalWrite(PTUNE_PIN, LOW);  // set  digital off to releae ATU 100 Tunner
 
}

// -------------------------------------------
// Provide a menu to select one of the bands
// -------------------------------------------
uint8_t band_select()
{
  #define  BASEX 18
  #define  BASEY 16
  #define  WIDTH 438
  #define  HEIGHT 240 
  #define  BUT_W 96  
  #define  BUT_H 54
  #define  BUT_SPACING_X 9 
  #define  BUT_SPACING_Y 3

  TFT_eSPI_Button band_button[16];
  tft.setFreeFont(&FreeSansBold9pt7b);
 
 // tft.setTextFont(2); 
  tft.setTextColor(TFT_SILVER,TFT_SILVER);

  tft.fillRect(BASEX, BASEY-2, WIDTH, HEIGHT, TFT_DARKGREEN);
  tft.drawRect(BASEX, BASEY-2, WIDTH, HEIGHT, TFT_SILVER);
  tft.drawRect(BASEX+1, BASEY-3, WIDTH-2, HEIGHT-2, TFT_SILVER);

  for(uint8_t row=0;row<4;row++) {
    for(uint8_t col=0;col<4;col++)
    {
      uint16_t color;
      if( current_band == (row*4+col) )
        color = TFT_RED;
      else
        color = TFT_DARKSILVER;

      band_button[row*4+col].initButton(&tft, 
        BASEX + 62 + col * (BUT_W + BUT_SPACING_X),  
        BASEY + 32 + row * (BUT_H + BUT_SPACING_Y), 
        BUT_W, BUT_H, TFT_BLACK, 
        color, TFT_WHITE, bands[row*4+col], KEY_TEXTSIZE);
      band_button[row*4+col].drawButton();

    }
  }

  // check which button was pressed:
  while(1) {
    uint8_t pressed = check_touch();

	  for (uint8_t i=0; i<16; i++) {
  	  if (pressed && band_button[i].contains(x, y)) {
        tft.fillRect(0,0,480,320,TFT_BLACK);
        refresh_required = true;  // refresh frequency display
        show_freq();
        drawKeypad();
        drawSMeter();
        text_show_bar();
        draw_lock();
        draw_band();
        draw_antena();
        bat_refresh = true;
        show_SYSpw();
        show_FTpw();
        return (i); // i contains the code for the band as used by the FT817/818
      }
	  }
  }

}

// -------------------------------------------
// Provide a menu to select one of the modes
// -------------------------------------------
uint8_t mode_select()
{
  #define  BASEX 80
  #define  BASEY 28
  #define  WIDTH 354
  #define  HEIGHT 208
  #define  BUT_W 110
  #define  BUT_H 60
  #define  BUT_SPACING_X 6
  #define  BUT_SPACING_Y 6

  TFT_eSPI_Button mode_button[16];
  
  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.setTextColor(TFT_SILVER,TFT_SILVER);

  tft.fillRect(BASEX-20, BASEY, WIDTH, HEIGHT, TFT_DARKGREEN);
  tft.drawRect(BASEX-20, BASEY, WIDTH, HEIGHT, TFT_SILVER);
  tft.drawRect(BASEX-19, BASEY-1, WIDTH, HEIGHT+2, TFT_SILVER);

  for(uint8_t row=0;row<3;row++) {
    for(uint8_t col=0;col<3;col++)
    {
      uint16_t color;
      if( current_mode == (row*3+col) )
        color = TFT_RED;
      else
        color = TFT_DARKSILVER;

      mode_button[row*3+col].initButton(&tft, 
        BASEX + 42 + col * (BUT_W + BUT_SPACING_X),
        BASEY + 38 + row * (BUT_H + BUT_SPACING_Y), 
        BUT_W, BUT_H, TFT_BLACK, 
        color, TFT_WHITE, modes[row*3+col], KEY_TEXTSIZE);

      mode_button[row*3+col].drawButton();


    }
  }

  // check which button was pressed:
  while(1) {
    uint8_t pressed = check_touch();

	  for (uint8_t i=0; i<9; i++) {
  	  if (pressed && mode_button[i].contains(x, y)) {
        tft.fillRect(0,0,480,320,TFT_BLACK);
        refresh_required = true;  // refresh frequency display
        show_freq();
        drawKeypad();
        drawSMeter();
        text_show_bar();
        draw_lock();
        draw_band();
        bat_refresh = true;
        show_SYSpw();
        show_FTpw();
        return (i); // i contains the code for the mode as used by the FT817/818
      }
	  }
  }

}

// ######################## TOUCH ########################

void getPoint()
{

  TS_Point p = ts.getPoint(); 
  // 480 x 320
 x = map(p.x, TS_MINX, TS_MAXX, 10, 470);
 y = map(p.y, TS_MINY, TS_MAXY, 10, 310);
}

// -------------------------------------------
// Checks if display is touched and returns
// the duration the screen is touched
// -------------------------------------------
uint8_t check_touch()
{	

	ulong starttime = millis();

	while(ts.touched())
  {
   	if(millis()-starttime > 300) {		  // long press (>300ms)
      getPoint();
      return 2;		
    }
  }

  if(millis()-starttime > 10 ) {	// short press(>10ms)
      getPoint();
		  return 1;
  }

	// no press
	return 0;

}


// ----------------------------
void draw_lock() {

  uint32_t color;
  int32_t x = 45;
  int32_t y = 204;
  tft.fillRect(x-40,y-33,75,46,TFT_BLACK);
 // locked = radio.getLock();
  if(locked) {
    color = TFT_RED;
    tft.fillRect(x,y-7,3,5,TFT_BLACK);
  }
  else {
    color = TFT_GREEN;
    tft.fillRect(x,y-7,3,5,TFT_BLACK);
  }

  tft.drawCircle(x+1,y-15,5,color);
  tft.drawCircle(x+1,y-15,6,color);
  tft.fillRect(x-8,y-12,19,12,color);
      
}

// ----------------------------
void drawKeypad()
{
 
  // Draw the keys
  for (uint8_t col = 0; col < NUM_OF_KEYS; col++) {
    uint8_t b = col + (menu_page-1)*NUM_OF_KEYS;
    update_menu(b);
  }

  // 480 x 320
 tft.drawRect(4,4,468,117,TFT_SILVER);
 tft.drawRect(4,123,468,44,TFT_SILVER);
 tft.drawRect(4,217,468,44,TFT_SILVER);
 tft.setFreeFont(&FreeSansBold9pt7b);
 
  // 480 x 320
  tft.fillRect(5,125,465,40,TFT_DARKBLUE);
  tft.setTextColor(TFT_WHITE, TFT_DARKBLUE);
  tft.drawCentreString("VFO",30,127,GFXFF);
  tft.drawCentreString("MODE",97,127,GFXFF);
  tft.drawCentreString("IPO",165,127,GFXFF);
  tft.drawCentreString("ATT",232,127,GFXFF);
  tft.drawCentreString("NB",300,127,GFXFF);
  tft.drawCentreString("AGC",367,127,GFXFF);
  tft.drawCentreString("STEPS",435,127,GFXFF);

 tft.fillRect(5,219,465,40,TFT_DARKBLUE);
 tft.setTextColor(TFT_WHITE, TFT_DARKBLUE);
 tft.drawCentreString("PWR",30,221,GFXFF);
 tft.drawCentreString("ANT",97,221,GFXFF);
 tft.drawCentreString("chg",165,221,GFXFF);
 tft.drawCentreString("CHG",232,221,GFXFF);
 tft.drawCentreString("Source",300,221,GFXFF);
 tft.drawCentreString("% Bat",367,221,GFXFF);

  for(uint8_t i=0;i<14;i++)
    field[i].refresh();

}

// --------------------------------------------------------
//  Check, which part of the frequency digits are touched
// --------------------------------------------------------
uint8_t  digits2edit(long old)
{ 
  uint8_t pressed = check_touch();
  if( pressed )     // touched?
  {
   // 480 x 320
   if( y > 176 && y < 218 ) {
    if(x > 220 && x < 284)  // left
    {
     return(3);
    }
    else if( x > 284 && x < 354 )  // middle
    {
      return(2);
    }
    else if( x > 354 )  // right
    {
    return(1);
    }
  }

  }
  return(old);    // nothing pressed

}

// -------------------------------------------
// Show the current frequency on the display
// -------------------------------------------
void show_freq()
{
  char buffer[20];
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  edit_frequ = digits2edit(edit_frequ); 

  if(edit_frequ > 0) {     // Edit mode
    f = radio.getFreqMode()*10;
   // control_power();
    long backup_frequ=f;    // backup current frequence
    long fneu=0;
    long faux = f;
    long fold=0;
  
    TFT_eSPI_Button exit_button[2];
    tft.setFreeFont(&FreeSansBold9pt7b);

    // 480 x 320
    tft.fillRect(40,218,400,100,TFT_DARKGREEN);
    tft.drawRect(38, 217, 404, 103, TFT_SILVER);
    tft.drawRect(39, 216, 402, 103, TFT_SILVER);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREEN); //TFT_DARKBLUE
    tft.drawCentreString("Move TX-FT818 freq. dial for adjust digits",240,227,GFXFF);
    exit_button[0].initButton(&tft, 353, 280, 150, 45, TFT_SILVER, 
                              TFT_DARKSILVER, TFT_WHITE, "CANCEL", KEY_TEXTSIZE);
    exit_button[0].drawButton();
    exit_button[1].initButton(&tft, 128, 280, 150, 45, TFT_SILVER, 
                              TFT_DARKSILVER, TFT_WHITE, "OK", KEY_TEXTSIZE);          
    exit_button[1].drawButton();

    uint32_t dialspeed = millis();
    tft.setFreeFont(&FreeSansBold18pt7b);

    do {

      // Check whether "OK" or "Cancel" was pressed: 
      uint8_t pressed = check_touch();

      if (pressed && exit_button[0].contains(x, y)) {     // Cancel button
        exit_button[0].press(true);  // tell the button it is pressed
        radio.setFreq(backup_frequ/10);  // restore old value
        tft.fillScreen(TFT_BLACK);
        drawKeypad();
        drawSMeter();
        text_show_bar();
        draw_lock();
        draw_band();
        draw_antena();
        bat_refresh = true;
        show_SYSpw();
        show_FTpw();

        exit_button[0].press(false);  // tell the button it is NOT pressed
        edit_frequ = 0;
        refresh_required = true;
        
        return;
      }
      else if (pressed && exit_button[1].contains(x, y)) {  // Ok button
        exit_button[1].press(true);  // tell the button it is pressed
        radio.setFreq(faux/10);
        tft.fillScreen(TFT_BLACK);
        drawKeypad();
        drawSMeter();
        text_show_bar();
        draw_lock();
        draw_band();
        draw_antena();
        bat_refresh = true;
        show_SYSpw();
        show_FTpw();
        exit_button[1].press(false);  // tell the button it is NOT pressed
        edit_frequ = 0;
        refresh_required = true;
        return;
      }

      fneu = radio.getFreqMode()*10;

      uint8_t old_digits = edit_frequ;
      edit_frequ = digits2edit(edit_frequ); 

      if(fneu != fold || edit_frequ != old_digits) {
        long tick;

        old_digits = edit_frequ;

        if (fneu-fold > 50)  tick=1;
        else if (fneu-fold < -50)  tick=-1;
        else tick=0;
   
        // speed up if knob is turned faster:
        if(tick != 0)
         if ( (millis() - dialspeed) < 100 ) {
          tick  = 10 * tick;
          dialspeed = millis();
        }

        if(tick != 0) {
          dialspeed = millis();
        }
  
        fold=fneu;

        if( edit_frequ == 2) 
          tick = tick * 1000;
        else if( edit_frequ == 3)
          tick = tick * 1000000;

        faux = faux + tick;

        // Check if new frequency vale is within valid range:
        if( faux < 100000)                          faux=470000000;
        else if (faux>56000000  && faux<67000000)   faux=76000000;
        else if (faux<76000000  && faux>67000000)   faux=56000000;
        else if (faux>154000000 && faux<280000000)  faux=420000000;
        else if(faux<420000000  && faux>280000000)  faux=154000000;
        else if (faux>470000000)                    faux=100000;
      
        tick=0;

        sprintf(buffer,"%ld",faux);

        int len = strlen(buffer);
        
        int x=405; int y=176; // 480 x 320
        //  Delete old digits:
        
        tft.fillRect(220,170,250,42,TFT_BLACK); // 480 x 320
       
        int i=1;
        int part=1;

        while(len>0) {
          if(i>3 && len>0) {
            x = x - tft.drawChar('.', x+18,y+28,GFXFF);
            part++;
            i=1;
          }
          else {
            if(part==edit_frequ)
              tft.setTextColor(TFT_RED,TFT_BLACK);    
            x = x - tft.drawChar(buffer[len-1], x+8,y+28,GFXFF);
            tft.setTextColor(TFT_WHITE,TFT_BLACK);    
            len--;
            i++;
          }
        }
      } 
    } while (edit_frequ > 0);

  }
  else {
    control_power();
    tft.setFreeFont(&FreeSansBold18pt7b);
    f = radio.getFreqMode()*10;
    if( (f != old_freq) || refresh_required) {
      refresh_required = false;
      updateTime = millis();

      sprintf(buffer,"%ld",f);

      int len = strlen(buffer);
       int x=405; int y=176; // 480 x 320
      //  Delete old digits:
      tft.fillRect(220,170,250,42,TFT_BLACK); // 480 x 320
      int i=1;
      int part=1;

      while(len>0) {
        if(i>3 && len>0) {
          tft.setTextColor(TFT_WHITE,TFT_BLACK);  
          x = x - tft.drawChar('.', x+18,y+28,GFXFF);
          part++;
          i=1;
        }
        else {
          if(part==edit_frequ)
            tft.setTextColor(TFT_RED,TFT_BLACK);

        if( f < hambands[current_band][0] || f > hambands[current_band][1] )
          tft.setTextColor(TFT_RED,TFT_BLACK);
          x = x - tft.drawChar(buffer[len-1], x+8,y+28,GFXFF);
          // tft.setTextColor(TFT_CYAN,TFT_BLACK);    TFT_WHITE
          tft.setTextColor(TFT_WHITE,TFT_BLACK); 
          len--;
          i++;
        }
      }
      get_nb_agc_lock();
      get_band();
      old_freq = f;
    }
  }
 
}


void antenna_setup() 
{
  #define  BASEX 5
  #define  BASEY 5
  #define  WIDTH 340
  #define  HEIGHT 230

  bool changed = false;

  TFT_eSPI_Button ant_button[8];

  tft.setFreeFont(&FreeSansBold9pt7b);

  tft.fillRect(BASEX+70, BASEY+12, WIDTH, HEIGHT, TFT_DARKGREEN);
  tft.drawRect(BASEX+70, BASEY+12, WIDTH, HEIGHT, TFT_SILVER);
  tft.drawRect(BASEX+71, BASEY+11, WIDTH-2, HEIGHT-1, TFT_SILVER);
  
  tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
  tft.drawString("HF", BASEX+130, BASEY+26, GFXFF);
  tft.drawString("6m", BASEX+239, BASEY+26, GFXFF);
  tft.drawString("Radio", BASEX+350, BASEY+26, GFXFF);
  tft.drawString("Air", BASEX+130, BASEY+100, GFXFF);
  tft.drawString("2m", BASEX+239, BASEY+100, GFXFF);
  tft.drawString("70cm", BASEX+346, BASEY+100,GFXFF);

  byte ant = radio.getAnt();    // EEPROM read

  if (radio.eepromValidData)
  {
    for(int i=0;i<6;i++) {
      ant_button[i].setLabelDatum(0,4,MC_DATUM);

      if( bitRead(ant,i) ) {
        if (i<3) {
            ant_button[i].initButton(&tft, BASEX+130+i*110 ,BASEY+66, 80, 35, TFT_BLACK,  // Fila 1 rear
            TFT_DARKBLUE, TFT_WHITE, "REAR", KEY_TEXTSIZE);
          }
        else  {
            ant_button[i].initButton(&tft, BASEX+130+(i-3)*110,BASEY+140, 80, 35, TFT_BLACK,    // Fila 2 rear
            TFT_DARKBLUE, TFT_WHITE, "REAR", KEY_TEXTSIZE);
          }
      }
      else {
        if (i<3) {
          ant_button[i].initButton(&tft, BASEX+130+i*110,BASEY+66, 80, 35, TFT_BLACK, 
                              TFT_DARKSILVER, TFT_WHITE, "FRONT", KEY_TEXTSIZE);
         }
        else {
          ant_button[i].initButton(&tft, BASEX+130+(i-3)*110,BASEY+140, 80, 35, TFT_BLACK, 
            TFT_DARKSILVER, TFT_WHITE, "FRONT", KEY_TEXTSIZE);
        }  
      }
      ant_button[i].drawButton();
    }
  }

  ant_button[6].initButton(&tft, 170, 210, 100, 40, TFT_SILVER, 
                              TFT_DARKSILVER, TFT_WHITE, "CANCEL", KEY_TEXTSIZE);          
  ant_button[6].drawButton();
  ant_button[7].initButton(&tft, 320, 210, 100, 40, TFT_SILVER, 
                              TFT_DARKSILVER, TFT_WHITE, "OK", KEY_TEXTSIZE);          
  ant_button[7].drawButton();

  while(1) {
    uint8_t pressed = check_touch();

    for (uint8_t i=0; i<6; i++) {
      
      if ( pressed && ant_button[i].contains(x, y) ) {
        changed = true;
        if( bitRead(ant,i) ) {
          ant = bitClear(ant,i);
          if (i<3) {
            ant_button[i].initButton(&tft, BASEX+130+i*110 ,BASEY+66, 80, 35, TFT_BLACK, 
            TFT_DARKSILVER, TFT_WHITE, "FRONT", KEY_TEXTSIZE);
          }
          else {
            ant_button[i].initButton(&tft, BASEX+130+(i-3)*110,BASEY+140, 80, 35, TFT_BLACK, 
            TFT_DARKSILVER, TFT_WHITE, "FRONT", KEY_TEXTSIZE);
          }
        } else {
          ant = bitSet(ant,i);
          if (i<3) {
            ant_button[i].initButton(&tft, BASEX+130+i*110 ,BASEY+66, 80, 35, TFT_BLACK, 
              TFT_DARKBLUE, TFT_WHITE, "REAR", KEY_TEXTSIZE);
          }
          else{
            ant_button[i].initButton(&tft, BASEX+130+(i-3)*110,BASEY+140, 80, 35, TFT_BLACK, 
            TFT_DARKBLUE, TFT_WHITE, "REAR", KEY_TEXTSIZE);
          }
           
          
        }
        ant_button[i].drawButton(); 
        delay(500); // debounce
      }
    
    }

    if (pressed && ant_button[6].contains(x, y)) {      // CANCEL button
      ant_button[6].press(true);  // tell the button it is pressed
      tft.fillScreen(TFT_BLACK);
      refresh_required = true;  // refresh frequency display
      show_freq();
      drawKeypad();
      drawSMeter();
      text_show_bar();
      draw_lock();
      draw_band();
      draw_antena();
      bat_refresh = true;
      show_SYSpw();
      show_FTpw();
      return;
    }

    else if (pressed && ant_button[7].contains(x, y)) {   // OK button
      ant_button[7].press(true);  // tell the button it is pressed

      // only write antenna config to EEPROM, if something was changed;
      if(changed)
        radio.setAnt(ant);

      tft.fillScreen(TFT_BLACK);
      refresh_required = true;  // refresh frequency display
      show_freq();
      drawKeypad();
      drawSMeter();
      text_show_bar();
      draw_lock();
      draw_band();
      get_antenna();
      bat_refresh = true;
      show_SYSpw();
      show_FTpw();
      return;
    } else {
      ant_button[6].press(false);  // tell the button it is NOT pressed
      ant_button[7].press(false); 
    }
  }

}

//---------------------------------------------------
//  check which button was pressed on the main page
//---------------------------------------------------
uint8_t check_menu_buttons()
{

  uint8_t pressed = check_touch();

 	uint8_t b = 0;  
      if ( pressed && (x > 17) && (x < 70) && (y > 170) && (y < 220) ){ // 480x320
    
      return(100);	//locked pressed
  }
    

	for (uint8_t col = 0; col < NUM_OF_KEYS; col++) {
	  b = col + (menu_page-1)*NUM_OF_KEYS;
    if (pressed && key[b].contains(x, y))
    {
      delay(50);  // debounce
		  return (b); // menu button pressed
    }
	}
  return -1;   // no button pressed

}

//------------------------------------
void button_events(uint8_t b) {
  static int8_t told_mode;
  static byte   told_pwr;

  tft.setFreeFont(LABEL1_FONT);

  if(b>=0 && b < NUM_OF_KEYS*MAX_MENU_PAGES) {
    key[b].initButton(&tft, KEY_X + (b-NUM_OF_KEYS*((menu_page-1))) * (KEY_W + KEY_SPACING_X),
        KEY_Y, // x, y, w, h, outline, fill, text
        KEY_W, KEY_H, TFT_WHITE, TFT_SILVER, TFT_WHITE,
        keyLabel[b], KEY_TEXTSIZE);
        key[b].drawButton();
  }

  if(b==5 + NUM_OF_KEYS*((menu_page-1)))
  {
    menu_page++;
    if(menu_page > MAX_MENU_PAGES)
      menu_page=1;
    delay(50);
    drawKeypad();
    bat_refresh = true;
    show_FTpw();
  }
  
  else if(b == 0)   // Band key
  {
    uint8_t band;
    band = band_select();
    if( band != current_band) {  // write only to EEPROM if band changed
    //  Serial.println("EEPROM WRITE: setBandVFO())");
      radio.setBandVFO(current_vfo, band);
    }
  }

  else if(b == 1)   // mode key
  {
    uint8_t mode;
    mode = mode_select();
    if( mode != current_mode && mode != 5) {  // write only to EEPROM if mode changed
                                              // and mode is not 5 (WFM)
      // Serial.println("EEPROM WRITE: setMode())");
      radio.setMode(mod_codes[mode]);
    }
    
    
  }

  else if(b == 2)   // IPO key
  {
   // Serial.println("EEPROM WRITE: toggleIPO()");
    radio.toggleIPO();
    bool ipo = radio.getIPO();
    if(ipo)
      keyStatus[2]=true;
    else
      keyStatus[2]=false;
  }
    
  else if(b == 3)   // ATT key
  {
   // Serial.println("EEPROM WRITE: toggleATT()");
    radio.toggleATT();
    bool att = radio.getATT();

    if(att)
      keyStatus[3]=true;
    else
      keyStatus[3]=false;
  }

  else if(b == 4) {  // TUN key

   // delay(500);
    // update_menu(4);
   
    if( keyStatus[4] ) {
      
      radio.PTT(false);
      delay(500); 
      current_mode = told_mode;
      if(told_pwr != TUNE_PWR) {
        radio.setPWR(told_pwr);    // restore previous power value 
       }
       if(told_mode != 8) { // PKT mode
       radio.setMode(mod_codes[told_mode]);   // restore previous mode
        field[1].drawField(modes[told_mode], TFT_YELLOW, TFT_DARKBLUE); 
       }
      keyStatus[4]=false;
      update_menu(4);
    }
    else {
      told_pwr = current_pwr;
      get_mode();
      told_mode = current_mode;
      current_mode = 8;
      current_pwr = TUNE_PWR;
      if(told_mode != current_mode){ 
        radio.setMode(mod_codes[current_mode]);   // set mode to PKT, if not already selected
        field[1].drawField(modes[current_mode], TFT_YELLOW, TFT_DARKBLUE); 
       }
      delay(500);
      if(told_pwr != current_pwr) {
        radio.setPWR(current_pwr);   // set power to tuning power, if not already selected     
        }
      delay(500);  
      byte res = radio.PTT(true);
      push_tunner(); // Active ATU 100 EXT
   
      if( res == 0xF0 ) {           // could not enable TX
         if(told_mode != 8) // 
          radio.setMode(mod_codes[told_mode]);   // restore previous mode
        if(told_pwr != TUNE_PWR) 
          radio.setPWR(told_pwr);    // restore previous power value
        byte res = radio.PTT(false);
        keyStatus[4]=false;
        update_menu(4); 

      }
      else {                         // TX switched on
        keyStatus[4]=true;
        update_menu(4);
      }
    }

  }

  else if(b == 6)   // VFO key
  {
    radio.toggleVFO();
    get_vfo();

    update_menu(6);
  }

  else if(b == 7)   // NB key
  {
   // Serial.println("EEPROM WRITE: toggleNB()");
    radio.toggleNB();
    bool nb = radio.getNB();
    if(nb){
      field[4].drawField("on", TFT_YELLOW, TFT_DARKBLUE);
      keyStatus[7]=true;}
    else{
      field[4].drawField("off", TFT_YELLOW, TFT_DARKBLUE);
      keyStatus[7]=false;}
      update_menu(7);  
  }

  else if(b == 8) {  // TX Power key
     current_pwr--;
    if(current_pwr < 0) current_pwr=3;
    radio.setPWR(current_pwr);
   // Serial.print("EEPROM WRITE: setPWR()   ");
    update_menu(8);
   
  }

  else if(b == 9) {   // antenna key
    antenna_setup(); 
    update_menu(9);
  }
  else if(b == 10) { // AGC key
    // Serial.println("EEPROM WRITE: setAGC()");
    current_agc++;
    if(current_agc > 3) current_agc=0;
    radio.setAGC(current_agc);
    field[5].drawField(agc[current_agc], TFT_YELLOW, TFT_DARKBLUE);
    update_menu(10);
  }

  else if(b == 12) {  // STP key

    if(current_mode == 4) {   // AM mode
      current_am_step++;
      if(current_am_step > 5) current_am_step = 0;
    }
    else if(current_mode == 5 || current_mode == 6 || current_mode == 8 ) {   // FM mode
      current_fm_step++;
      if(current_fm_step > 7) current_fm_step = 0;
    }
    else {
      current_ssb_step++;     // SSB mode
      if(current_ssb_step > 2) current_ssb_step = 0;
    }
    radio.setSteps(current_ssb_step << 6 | current_am_step << 3 | current_fm_step);
    update_menu(12);
  }

  else if(b == 13) {  // CHG key
   // Serial.println("EEPROM WRITE: setChargeTime()");
    current_chg++;
    if(current_chg > 2) current_chg=0;
    radio.setChargeTime(current_chg);
    field[9].drawField(chargeTimes[current_chg], TFT_YELLOW, TFT_DARKBLUE);
    update_menu(13);
  }

  else if(b == 14) {  // Charging enable key
    // Serial.println("EEPROM WRITE: enableCharging()");

  
    if(charging) {
      radio.enableCharging(false);
      charging = false;
       keyStatus[14]=false;
       field[10].drawField("off", TFT_YELLOW, TFT_DARKBLUE);
     
      
    }
    else {
      radio.enableCharging(true);
      charging = true;
      keyStatus[14]=true;
      field[10].drawField("on", TFT_YELLOW, TFT_DARKBLUE);
    }

    update_menu(14);
  }

  else if(b == 15) { // dummy key '4'
    delay(500);
    keyStatus[15]=false;
    update_menu(15);
  }

  else if(b == 16) {  // dummy key '5'
    delay(500);
    keyStatus[16]=false;
    update_menu(16);
  }

  else if(b==100)
  {
    tft.drawRect(18,171,59,39,TFT_SILVER);
    tft.drawRect(19,172,57,37,TFT_SILVER);
    if(locked){
          radio.lock(false);
          locked = false;
        }
    else {
      radio.lock(true);
      locked = true;
    }
    draw_lock();
  }

}


// ------------------------------
//    update menu button
// ------------------------------
void update_menu(uint8_t b) {

  uint8_t col = b % NUM_OF_KEYS;

// if button is currently not shown in menu, skip. Otherwise changes would
// apply to the button of another menu page at same column
  if( (int) (b/NUM_OF_KEYS + 1)  != menu_page) return;

  tft.setFreeFont(LABEL1_FONT);

  if(keyStatus[b])
    key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
      KEY_Y, KEY_W, KEY_H, TFT_WHITE, keyColor_active[b], TFT_WHITE,
      keyLabel[b], KEY_TEXTSIZE);
  else
    key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
      KEY_Y, KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
      keyLabel[b], KEY_TEXTSIZE);

  key[b].drawButton();

}

// ------------------------------
//    release all menus keys
// ------------------------------
void release_keys() {
  
  for(uint8_t b=0; b<11; b++) {
    uint8_t col = b % NUM_OF_KEYS;
    if( (int) (b/NUM_OF_KEYS + 1)  != menu_page) return;
    tft.setFreeFont(LABEL1_FONT);
    key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
      KEY_Y, KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
      keyLabel[b], KEY_TEXTSIZE);
    key[b].drawButton();
  }  
}



void sec2time(char *buf, uint32_t seconds)
{

  ldiv_t res;
  int secs, minutes, hours;

  res = ldiv( seconds, 60 );
  secs = res.rem;

  sprintf(buf, "%02d:%02d", res.quot, secs);

  return;

}