#include "mbed.h"
#include "N5110.h"
#include "MMA8452.h"

//Pin assignment format:  lcd(IO, Ser_TX, Ser_RX, MOSI, SCLK, PWM)  
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);

//I2C:         sda,   scl
MMA8452 accel(PB_14, PB_13);

char idBuffer[14];
char sense[14];
char xAxisBuffer[14];
char yAxisBuffer[14];
char zAxisBuffer[14];

int main(){
    lcd.init(LPH7366_1);
    accel.init(FOUR_G);
    sprintf(idBuffer, "Device ID: %02x", accel.get_ID());   //read and store device id (hex) in buffer
    sprintf(sense, "Sense: %.1f", accel.sensitivitySetting());     //read and store current sensitivity setting
    ThisThread::sleep_for(10ms);

    while(1){
        lcd.clear();
        lcd.printString(idBuffer, 0, 0);                        //print device id in hex
        lcd.printString(sense, 0, 1);                           //print device sensitivity

        /*sprintf(xAxisBuffer, "Xaxis: %.2f", accel.readValues().x);  //read and store x axis values in buffer
        sprintf(yAxisBuffer, "yaxis: %.2f", accel.readValues().y);  //read and store y axis values in buffer
        sprintf(zAxisBuffer, "zaxis: %.2f", accel.readValues().z);  //read and store z axis values in buffer*/

        /*sprintf(xAxisBuffer, "Xaxis: %.2f", accel.readAngles().x);  //read and store x axis values in buffer
        sprintf(yAxisBuffer, "yaxis: %.2f", accel.readAngles().y);  //read and store y axis values in buffer
        sprintf(zAxisBuffer, "zaxis: %.2f", accel.readAngles().z);  //read and store z axis values in buffer*/

        /*lcd.printString(xAxisBuffer, 0, 2);     //print x axis values
        lcd.printString(yAxisBuffer, 0, 3);     //print y axis values
        lcd.printString(zAxisBuffer, 0, 4);     //print z axis values*/

        //test the portrait/landscape orientation detection
        if(accel.facingLeft() == true){
            lcd.printString("Left", 0, 3);

        }else if(accel.facingRight() == true){
            lcd.printString("Right", 0, 3);

        }else if(accel.isUpright() == true){
            lcd.printString("Upright", 0, 3);

        }else if(accel.isInverted() == true){
            lcd.printString("Downward", 0, 3);

        }else if(accel.isFlat() == true){
            lcd.printString("Flat", 0, 3);
        }

        lcd.refresh();
        ThisThread::sleep_for(100ms);
    }
}

