// demo of ..
#include <TimerOne.h>
#include <Streaming.h>

void setup()
{
    Serial.begin(38400);

    Timer1.initialize(100000);
    Timer1.attachInterrupt( timerIsr );
    cout << "hello world" << endl;
   
}

void loop()
{
    // add code here
}

void timerIsr()
{
    // add code here
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
