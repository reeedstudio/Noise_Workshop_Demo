/*-----------------------------------------------------------------------
** demo of noise workshop demo, for chaihuo
** loovee 2013-6-7
** https://github.com/reeedstudio/Noise_Workshop_Demo
**
** grove - camera plug to   D6(and D7)
** grove - button plug to   D2
** grove - led bar plug to  D8(and D9)
** grove - sound sensor plug to A0
** grove - buzzer plug to D3
**---------------------------------------------------------------------*/

#include <TimerOne.h>
#include <Streaming.h>
#include <LED_Bar.h>

#include "NWDfs.h"

LED_Bar myLED;

int getNoise()
{

    unsigned int vol = 0;
    for(int i=0; i<32; i++)
    {
        vol += analogRead(A0);
    }

    vol = vol>>5;
    
    vol = vol>VOLINH ? VOLINH : (vol<VOLINL ? VOLINL : vol);
    vol = map(vol, VOLINL, VOLINH, VOLOUTL, VOLOUTH);
    
    return vol;
    
}

void setBar(int in)
{
    in = in>10 ? 10 : (in<1 ? 1 : in);
    
    unsigned int tmp = 0;
    for(int i=0; i<in; i++)
    {
        tmp += 0x01<<(9-i);
        myLED.set_LED_Index(tmp);
    }  
    
}

void setup()
{
    Serial.begin(38400);
    myLED.set_LED_Index(0);
    
    pinMode(PINBUTTON, INPUT);
    pinMode(PINBUZZER, OUTPUT);

    cout << "hello world" << endl;
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
}

int vol     = 0;
int volBuf  = 0;

void loop()
{
    // add code here
    
    vol = getNoise();

    if(vol != volBuf)
    {
        volBuf = vol;
        setBar(volBuf);
        cout << volBuf << endl;
    }
    
    
    if(BTNSTATE())
    {
        delay(10);
        if(BTNSTATE())
        {
            while(BTNSTATE());
            BEEPON();
            delay(200);
            BEEPOFF();
        }
    }
    
    delay(10);

}

void timerIsr()
{
    // add code here
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
