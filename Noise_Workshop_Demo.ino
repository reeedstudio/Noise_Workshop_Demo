/*-----------------------------------------------------------------------------------
** demo of noise workshop demo, for chaihuo
** loovee 2013-6-7
** https://github.com/reeedstudio/Noise_Workshop_Demo
**
** grove - camera plug to   D0(&D1)
** grove - button plug to   D2
** grove - led bar plug to  D8(& D9)
** grove - sound sensor plug to A0
** grove - buzzer plug to D6
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

File logFile;

LED_Bar myLED;

char fileNameTemp[20];

int fileNum = 0;

void createLogFile()
{
    if (!SD.exists("log.txt"))            // create already
    {
        logFile = SD.open("log.txt", FILE_WRITE);
        logFile.close();
    }
}

void writeLogFile(int n)
{
    logFile = SD.open("log.txt", O_WRITE);
    char stmp[20];
    
    if(n<10)
    {
        sprintf(stmp, "NOISE00%d: %d\r\n", fileNum, n);
    }
    else if(n<100)
    {
        sprintf(stmp, "NOISE0%d: %d\r\n", fileNum, n);
    }
    else
    {
        sprintf(stmp, "NOISE%d: %d\r\n", fileNum, n);
    }
    
    logFile.write(stmp);
    logFile.close();
}

char *getFileName(int n)
{

    if(n<10)
    {
        sprintf(fileNameTemp, "NOISE00%d.jpg", n);
    }
    else if(n<100)
    {
        sprintf(fileNameTemp, "NOISE0%d.jpg", n);
    }
    else
    {
        sprintf(fileNameTemp, "NOISE%d.jpg", n);
    }
    
    return fileNameTemp;
}

int getFileNum()
{
    int n=0;

    while(1)
    {

        if (SD.exists(getFileName(n)))
        {
#if __Debug
            cout << fileNameTemp << " exist" << endl;
#endif
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

    unsigned int vol_t = 0;
    for(int i=0; i<32; i++)
    {
        vol_t += analogRead(A0);
    }

    vol_t = vol_t>>5;

    vol_t = vol_t>VOLINH ? VOLINH : (vol_t<VOLINL ? VOLINL : vol_t);
    vol_t = map(vol_t, VOLINL, VOLINH, VOLOUTL, VOLOUTH);

    return vol_t;

}

void setBar(int vol_t)
{
    vol_t = vol_t>10 ? 10 : (vol_t<1 ? 1 : vol_t);

    unsigned int tmp = 0;
    for(int i=0; i<vol_t; i++)
    {
        tmp += 0x01<<(9-i);
        myLED.set_LED_Index(tmp);
    }

}

void setup()
{
    UART.begin(115200);

#if __Debug && defined(__AVR_ATmega32U4__)
    Serial.begin(115200);
    while(!Serial);
#endif
    myLED.set_LED_Index(0);

    pinMode(PINBUTTON, INPUT);
    pinMode(PINBUZZER, OUTPUT);
    

    BEEPON();
    delay(500);
    BEEPOFF();

#if __Debug
    cout << "hello world" << endl;
#endif
    scInit();

    fileNum = getFileNum();
    
    createLogFile();
#if __Debug
    cout << "fileNum = " << fileNum << endl;
#endif
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
    }

    if(BTNSTATE())
    {
        delay(10);
        if(BTNSTATE())
        {
            while(BTNSTATE());
            long time1 = millis();
            BEEPON();
            delay(50);
            BEEPOFF();
            
            writeLogFile(vol);
            takePhoto(getFileName(fileNum++));
#if __Debug
            cout << endl << "take over: " << fileNameTemp << endl;
#endif

            if((millis()-time1) > 1000)     // take ok
            {
                BEEPON();
                delay(50);
                BEEPOFF();
                delay(100);
                BEEPON();
                delay(50);
                BEEPOFF();
            }
            else                            // take err
            {
                BEEPON();
                delay(500);
                BEEPOFF();
                delay(100);
                BEEPON();
                delay(500);
                BEEPOFF();
            }
        }
    }

    delay(10);

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
