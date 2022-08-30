#include "mbed.h"
#include "N5110.h"
#include "MMA8452.h"

//Pin assignment format:  lcd(IO, Ser_TX, Ser_RX, MOSI, SCLK, PWM)  
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);

//I2C:         sda,   scl
MMA8452 accel(PB_14, PB_13);
DigitalIn button_a(PH_1);

char idBuffer[14];
char sense[14];
char xAxisBuffer[14];
char yAxisBuffer[14];
char zAxisBuffer[14];

int main(){
    lcd.init(LPH7366_1);
    accel.init(EIGHT_G);
    button_a.mode(PullNone);
    sprintf(idBuffer, "Device ID: %02x", accel.get_ID());               //read and store device id (hex) in buffer
    sprintf(sense, "Sense: %.2f", accel.sensitivitySetting());          //read and store current sensitivity setting
    ThisThread::sleep_for(10ms);

    while(1){
        
        while(button_a == false){
            lcd.clear();
            lcd.printString(idBuffer, 0, 0);                            //print device id in hex
            lcd.printString(sense, 0, 1);                               //print device sensitivity
            lcd.refresh();
        }

        while(button_a == true){};                                      //wait for button to reset back to false

        //test read the measured g values in each axis
        while(button_a == false){
            sprintf(xAxisBuffer, "Xaxis: %.2f", accel.readValues().x);  //read and store x axis values in buffer
            sprintf(yAxisBuffer, "yaxis: %.2f", accel.readValues().y);  //read and store y axis values in buffer
            sprintf(zAxisBuffer, "zaxis: %.2f", accel.readValues().z);  //read and store z axis values in buffer
            lcd.clear();
            lcd.printString("G_Value Test", 0, 0);
            lcd.printString(xAxisBuffer, 0, 1);
            lcd.printString(yAxisBuffer, 0, 2);
            lcd.printString(zAxisBuffer, 0, 3);
            lcd.refresh();
            ThisThread::sleep_for(200ms);
        }

        while(button_a == true){};

        //test read the measured angle values (degrees) in each axis
        while(button_a == false){
            sprintf(xAxisBuffer, "Xaxis: %.2f", accel.readAngles().x);  //read and store x axis values in buffer
            sprintf(yAxisBuffer, "yaxis: %.2f", accel.readAngles().y);  //read and store y axis values in buffer
            sprintf(zAxisBuffer, "zaxis: %.2f", accel.readAngles().z);  //read and store z axis values in buffer
            lcd.clear();
            lcd.printString("Angle Test", 0, 0);
            lcd.printString(xAxisBuffer, 0, 1);
            lcd.printString(yAxisBuffer, 0, 2);
            lcd.printString(zAxisBuffer, 0, 3);
            lcd.refresh();
            ThisThread::sleep_for(200ms);
        }

        while(button_a == true){};

        //test orientation detection
        while(button_a == false){
            lcd.clear();
            lcd.printString("Orientation", 0, 0);

            //test the portrait/landscape orientation detection
            if(accel.facingLeft() == true){
                lcd.printString("Left", 0, 2);

            }else if(accel.facingRight() == true){
                lcd.printString("Right", 0, 2);

            }else if(accel.isUpright() == true){
                lcd.printString("Upright", 0, 2);

            }else if(accel.isInverted() == true){
                lcd.printString("Downward", 0, 2);

            }else if(accel.isFlat() == true){
                lcd.printString("Flat", 0, 2);
            }
            
            lcd.refresh();
            ThisThread::sleep_for(200ms);
        }

        while(button_a == true){};
    }
}

