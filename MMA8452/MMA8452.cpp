#include "mbed.h"
#include "MMA8452.h"
#include <cstdint>

MMA8452:: MMA8452(PinName const sda, PinName const scl){
    i2c = new I2C(sda,scl);                     // create new I2C instance and initialise
    i2c->frequency(400000);                     // I2C Fast Mode - 400kHz
}

void MMA8452::init(Range_setting const range){
    i2c->frequency(400000);                     // set Fast Mode I2C frequency
    get_ID();
    deviceStandby();                            //put device into standby during config

    sendByteToRegister(0x00, XYZ_DATA_CFG);     //initialise XYZ_DATA_CFG register
    
    //config portrait/lanscape
    sendByteToRegister(0x40, PL_CFG);
    sendByteToRegister(0x50, PL_COUNT);

    setRange(range);                            //set the range and sensitivity of the device, 2g, 4g, 8g
    deviceWake();
}

char MMA8452::get_ID(){
    char ID = readByteFromRegister(WHO_AM_I);

    if (ID != 0x2A) {                             // if correct ID not found, flash error message
        printf("\nError >> Incorrect device ID");
    }

    return ID; 
}

float MMA8452::sensitivitySetting(){
    return SENSITIVITY;
}

Acceleration MMA8452::readValues(){
    // acceleration data stored in 6 registers (0x01 to 0x06)
    // device automatically increments register, so can read 6 bytes starting from OUT_X_MSB
    char data[6];
    readBytesFromRegister(OUT_X_MSB,6,data);

    char x_MSB = data[0];  // extract MSB and LSBs for x,y,z values
    char x_LSB = data[1];
    char y_MSB = data[2];
    char y_LSB = data[3];
    char z_MSB = data[4];
    char z_LSB = data[5];

    // [0:7] of MSB are 8 MSB of 12-bit value , [7:4] of LSB are 4 LSB's of 12-bit value
    // need to type-cast as numbers are in signed (2's complement)
    int x = (int16_t) (x_MSB << 8) | x_LSB;  // combine bytes
    x >>= 4;                                 // are left-aligned, so shift 4 places right to right-align

    int y = (int16_t) (y_MSB << 8) | y_LSB;
    y >>= 4;

    int z = (int16_t) (z_MSB << 8) | z_LSB;
    z >>= 4;

    Acceleration acc;
    
    acc.x = x/SENSITIVITY;
    acc.y = y/SENSITIVITY;
    acc.z = z/SENSITIVITY;
    
    return acc;
}

Acceleration MMA8452::readAngles(){
    float x_raw = readValues().x;
    float y_raw = readValues().y;
    float z_raw = readValues().z;

    Acceleration angle;
    angle.x = (90 * x_raw);
    angle.y = (90 * y_raw);
    angle.z = (90 * z_raw);

    return angle;
}

bool MMA8452::facingLeft(){
    if(readPLStat() == 3){
        return true;
    }else{
        return false;
    }
}

bool MMA8452::facingRight(){
    if(readPLStat() == 2){
        return true;
    }else{
        return false;
    }
}

bool MMA8452::isUpright(){
    if(readPLStat() == 0){
        return true;
    }else{
        return false;
    }
}

bool MMA8452::isInverted(){
    if(readPLStat() == 1){
        return true;
    }else{
        return false;
    }
}

bool MMA8452::isFlat(){
    if(readPLStat() == 0x40){
        return true;
    }else{
        return false;
    }
}


/* Private functions */
void MMA8452::deviceStandby(){
    char data = readByteFromRegister(CTRL_REG1);    // get current value of register
    data &= ~(1<<0);                                // clear bit 0 (p37 datasheet)
    sendByteToRegister(data,CTRL_REG1);
}

void MMA8452::deviceWake(){
    char data = readByteFromRegister(CTRL_REG1);
    data |= (1<<0);                             // set bit 0 in CTRL_REG1
    sendByteToRegister(data,CTRL_REG1);
}

void MMA8452::setRange(Range_setting const range){
    char cmd;

    if(range == TWO_G){
        cmd = 0x00;        //set bit 0 to 0, set bit 1 to 0 - 2g range 0x00
        SENSITIVITY = 1024.0;

    }else if(range == FOUR_G){
        cmd = 0x01;        //set bit 0 to 1, set bit 1 to 0 - 4g range 0x01
        SENSITIVITY = 512.0; 

    }else if(range == EIGHT_G){
        cmd = 0x02;        //set bit 0 to 0, set bit 1 to 1 - 8g range 0x02
        SENSITIVITY = 256.0;
    }
    
    sendByteToRegister(cmd,XYZ_DATA_CFG);
}

char MMA8452::readPLStat(){
    char PL_status = readByteFromRegister(PL_STATUS);

    //test for lock out
    if(PL_status & 0x40){
        return 0x40;
    }else{
        PL_status = (PL_status & 0x06) >> 1;
        return PL_status;
    }

}

char MMA8452::readByteFromRegister(char reg){
    int nack = i2c->write(MMA8452_W_ADDRESS,&reg,1,true);  // send the register address to the slave
    // true as need to send repeated start condition (5.10.1 datasheet)
    // http://www.i2c-bus.org/repeated-start-condition/
    if (nack)
        printf("\nError >>  No acknowldgement : Error Index 01");

    char rx;
    nack = i2c->read(MMA8452_R_ADDRESS,&rx,1);  // read a byte from the register and store in buffer
    if (nack)
        printf("\nError >>  No acknowldgement : Error Index 02");

    return rx;
}

void MMA8452::readBytesFromRegister(char reg,int numberOfBytes,char bytes[]){
    int nack = i2c->write(MMA8452_W_ADDRESS,&reg,1,true);  // send the slave write address and the configuration register address
    // true as need to send repeated start condition (5.10.1 datasheet)
    // http://www.i2c-bus.org/repeated-start-condition/

    if (nack){
        printf("\nError >>  No acknowldgement : Error Index 03");
    }

    nack = i2c->read(MMA8452_R_ADDRESS,bytes,numberOfBytes);  // read bytes
    if (nack){
        printf("\nError >>  No acknowldgement : Error Index 04");
    }
}

void MMA8452::sendByteToRegister(char byte,char reg){
    char data[2];
    data[0] = reg;
    data[1] = byte;

    int nack = i2c->write(MMA8452_W_ADDRESS,data,2); // send the register address, followed by the data

    if (nack){
        printf("\nError >>  No acknowldgement : Error Index 05");
    }
}
