// demo of noise workshop demo, for chaihuo
// loovee 2013-6-7 https://github.com/reeedstudio/Noise_Workshop_Demo
// grove - camera plug to
// grove - button plug to
// 
#include <TimerOne.h>
#include <Streaming.h>
#include <LED_Bar.h>

LED_Bar myLED;

#include "NWDfs.h"

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
    
    myLED.set_LED_Index(0b000001101010101);
    delay(1000);
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
    
    delay(10);

}

void timerIsr()
{
    // add code here
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
