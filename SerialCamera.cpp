/*
  SerialCamera.cpp
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.

  Author:Loovee
  2013-06-07

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <SD.h>
#include <Streaming.h>

#include "SerialCamera.h"
#include "NWDfs.h"

File myFile;

#define PIC_BUF_LEN 96                                      // data length of each read

int picNameNum = 0;

void Restart()
{
    char cmdRST[] = {0x56, 0x00, 0x26, 0x00};                      //Reset command

    sendCmd(cmdRST, 4);
    delay(1000);

    while(UART.available() > 0)
    {
        UART.write(UART.read());
    }
#if __Debug
    cout << "Camera initialization done.";
#endif
}

void Capture()
{
    char cmdCAP[] = {0x56,0x00,0x36,0x01,0x00};                 //Take picture command
    sendCmd(cmdCAP, 5);
    delay(50);
    while(UART.available() > 0)
    {
        UART.read();
    }
}

void ContCapture()
{
    char cmdConti[] = {0x56,0x00,0x36,0x01,0x02};               //Take next picture command
    sendCmd(cmdConti, 5);

    delay(50);

    while(UART.available() > 0)
    {
        UART.write(UART.read());
    }
}

void GetData(char * fileName)
{
    unsigned int picTotalLen;

    char cmdGetLen[] = {0x56, 0x00, 0x34, 0x01, 0x00};          // Read JPEG file size command

    char cmdGetDat[] = {0x56, 0x00, 0x32, 0x0c, 0x00,           // Read JPEG data command
        0x0a, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
    0x0a};

    sendCmd(cmdGetLen, 5);
    delay(50);

    for(char i = 0; i < 7; i++)
    {
        UART.read();
    }

    picTotalLen = 0;

    int high = UART.read();
    picTotalLen |= high ;

    int low  = UART.read();
    picTotalLen = picTotalLen << 8 ;
    picTotalLen |= low ;                                        // get the length of picture

    unsigned int addr = 0;
    cmdGetDat[8]      = 0;
    cmdGetDat[9]      = 0;

    unsigned int count = picTotalLen / PIC_BUF_LEN;
    char tail = picTotalLen % PIC_BUF_LEN;
    cmdGetDat[13] = PIC_BUF_LEN;                                // the length of each read data

    myFile = SD.open(fileName, FILE_WRITE);

    if(!myFile)
    {
#if __Debug
        cout << "myFile open fail...";
#endif
    }
    else
    {
        for(char i = 0; i < count; i++)                         // get and save count*PIC_BUF_LEN data
        {
            sendCmd(cmdGetDat,16);
            delay(10);
            readCamSaveToFile(myFile, PIC_BUF_LEN);
            addr += PIC_BUF_LEN;
            cmdGetDat[8] = addr >> 8;
            cmdGetDat[9] = addr & 0x00FF;
        }

        cmdGetDat[13] = tail;                                   // get rest part of the pic data
        sendCmd(cmdGetDat,16);
        delay(10);
        readCamSaveToFile(myFile, tail);
    }

    myFile.close();

}

void readCamSaveToFile(File &myFile, int toBeReadLen)
{
    char readLen = 0;

    for(char i = 0; i < 5; i++)                  // read the signal that tells us that it's ready to send data
    {
        UART.read();
    }

    while(readLen < toBeReadLen)                // read and store the JPG data that starts with 0xFF 0xDB
    {
        myFile.write(UART.read());
        readLen++;
    }

    for(char i = 0; i < 5; i++)                 // read the signal of successful sending
    {
        UART.read();
    }
}

void sendCmd(char *cmd, int cmd_len)
{
    for(int i = 0; i < cmd_len; i++)
    {
        UART.print(cmd[i]);
    }
}

unsigned char scInit()
{
    pinMode(10,OUTPUT);          // CS pin of SD Card Shield
   
    if (!SD.begin(4))
    {
#if __Debug
        cout << "initialzation failed";
#endif

        while(1);
        return 0;
    }
#if __Debug
    cout << "initialization done.";
#endif
    Restart();
    return 1;
}

unsigned char takePhoto(char *fileName)
{
    static int fixN = 0;
    
    if(fixN == 0)
    {
        Capture();
    }
    else
    {
        ContCapture();
    }
    fixN++;
    GetData(fileName);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/