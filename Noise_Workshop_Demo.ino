/*-----------------------------------------------------------------------------------
** demo of noise workshop demo, for chaihuo
** loovee 2013-6-7
** https://github.com/reeedstudio/Noise_Workshop_Demo
**
** grove - camera plug to   D6(& D7)
** grove - button plug to   D2
** grove - led bar plug to  D8(& D9)
** grove - sound sensor plug to A0
** grove - buzzer plug to D3
**
** you need to change something in this file:
** in \hardware\arduino\cores\arduino\HardwareSerial.cpp
** you can find: #define SERIAL_BUFFER_SIZE 64
** modify it to: #define SERIAL_BUFFER_SIZE 128
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**--------------------------------------------------------------------------------*/

#include <TimerOne.h>
#include <Streaming.h>
#include <LED_Bar.h>
#include <SD.h>
#include <EEPROM.h>

#include "SerialCamera.h"
#include "NWDfs.h"

LED_Bar myLED;

int fileNum = 0;


char *getFileName(int n)
{
    char filename[20];
    if(n<10)
    {
        sprintf(filename, "NOISE00%d.txt", n);
    }
    else if(n<100)
    {
        sprintf(filename, "NOISE0%d.txt", n);
    }
    else
    {
        sprintf(filename, "NOISE%d.txt", n);
    }
    
    return filename;
}

int getFileNum()
{
    int n=0;
    char filename[20];
    while(1)
    {

        if (SD.exists(getFileName(n)))
        {
            cout << filename << " exist" << endl;
            n++;
        }
        else
        {
            return n;
        }
    }

}

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

void setBar(int vol)
{
    vol = vol>10 ? 10 : (vol<1 ? 1 : vol);

    unsigned int tmp = 0;
    for(int i=0; i<vol; i++)
    {
        tmp += 0x01<<(9-i);
        myLED.set_LED_Index(tmp);
    }

}

void setup()
{
    Serial.begin(115200);
    myLED.set_LED_Index(0);

    pinMode(PINBUTTON, INPUT);
    pinMode(PINBUZZER, OUTPUT);

    cout << "hello world" << endl;
    // Timer1.initialize(1000);
    // Timer1.attachInterrupt(timerIsr);

    scInit();

    fileNum = getFileNum();

    cout << "fileNum = " << endl;

}

int vol     = 0;
int volBuf  = 0;
int fixN = 0;

void loop()
{
    // add code here

    vol = getNoise();

    if(vol != volBuf)
    {
        volBuf = vol;
        setBar(volBuf);
        // cout << volBuf << endl;
    }


    if(BTNSTATE())
    {
        delay(10);
        if(BTNSTATE())
        {
            while(BTNSTATE());
            BEEPON();

            char fileName[20];

            takePhoto(getFileName(fileNum++));
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
