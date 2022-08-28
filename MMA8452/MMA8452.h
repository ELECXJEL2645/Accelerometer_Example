#ifndef MMA8452_h
#define MMA8452_h

#include "mbed.h"

//register definitions
//From Table 11 (Register Address map) of the data sheet
#define STATUS 0x00         //Read only : real time status
#define OUT_X_MSB 0x01      //Read only : 8 MSB's of 12 bit sample
#define OUT_X_LSB 0x02      //Read only : 4 LSB's of 12 bit sample
#define OUT_Y_MSB 0x03      //Read only : 8 MSB's of 12 bit sample
#define OUT_Y_LSB 0x04      //Read only : 4 LSB's of 12 bit sample
#define OUT_Z_MSB 0x05      //Read only : 8 MSB's of 12 bit sample
#define OUT_Z_LSB 0x06      //Read only : 4 LSB's of 12 bit sample
#define SYSMOD 0x0B         //Read only : current system mode
#define WHO_AM_I 0x0D       //Read only : Device ID (0x2A)
#define XYZ_DATA_CFG 0x0E   //Read/Write : HPF data out and dynamic range settings
#define PL_STATUS 0x10      //Read only : Landscape/Portrait orientation status
#define PL_CFG 0x11         //Read/Write : Landscape/Portrait configuration
#define PL_COUNT 0x12       //Read only : Landscape/Portrait debounce counter
#define PL_BF_ZCOMP 0x13    //Read only : Back-Front, Z-Lock Trip threshold
#define P_L_THS_REG 0x14    //Read/Write : Portrait to Landscape Trip Angle is 29°
#define FF_MT_CFG 0x15      //Read/Write : Freefall/Motion functional block configuration
#define FF_MT_SRC 0x16      //Read only : Freefall/Motion event source register
#define FF_MT_THS 0x17      //Read/Write : Freefall/Motion threshold register
#define FF_MT_COUNT 0X18    //Read/Write : Freefall/Motion debounce counter
#define ASLP_COUNT 0x29     //Read/Write : Counter setting for Auto-SLEEP
#define CTRL_REG1 0x2A      //Read/Write : ODR = 800 Hz, STANDBY Mode
#define CTRL_REG2 0x2B      //Read/Write : Sleep Enable, OS Modes, RST, ST
#define CTRL_REG3 0x2C      //Read/Write : Wake from Sleep, IPOL, PP_OD
#define CTRL_REG4 0x2D      //Read/Write : Interrupt enable register
#define CTRL_REG5 0x2E      //Read/Write : Interrupt pin (INT1/INT2) map

//From Table 13 (SYSMOD Description) of the data sheet
//Additionally refer to Chapter 4
#define STANDBY 0x00        //SYSMOD value = 0b00000000 (default)
#define WAKE 0x01           //SYSMOD value = 0b00000001
#define SLEEP 0x02          //SYSMOD value = 0b00000010

/*The break out board ties SA0 pin high as default, though this can be changed
* through the use of a solder blob to tie it to ground, changing the I2C address of the 
* device as required. More info at:
* https://learn.sparkfun.com/tutorials/mma8452q-accelerometer-breakout-hookup-guide */
#define MMA8452_ADDRESS 0x1D    //SA0 pin tied high (default)
//#define MMA8452_ADDRESS 0x1C    //if SA0 pin tied low
#define MMA8452_W_ADDRESS 0x3A
#define MMA8452_R_ADDRESS 0x3B

typedef struct Acceleration Acceleration;

enum Range_setting{
    TWO_G,
    FOUR_G,
    EIGHT_G,

};

struct Acceleration {
    float x;
    float y;
    float z;
};

class MMA8452{
public:
    //create accelerometer object
    //       Slave data line , Slave clock line
    MMA8452(PinName const sda, PinName const scl);

    //Powers up the accelerometer, sets 100 Hz update rate and ±4g scale
    void init(Range_setting const range);

    //get the ID address of the slave device
    char get_ID();
    
    //reads the current sensitivity setting, 8G -> 256, 4G -> 512, 2G -> 1024
    float sensitivitySetting();

    /*Reads the x,y,z values in g's and returns an
    * Acceleration structure with x,y,z members (float)*/
    Acceleration readValues();

    /*Reads the x,y,z values in degrees and returns an
    * Acceleration structure with x,y members (float)*/
    Acceleration readAngles();

    //check if orentation is left, returns true if so
    bool facingLeft();

    //check if orentation is right, returns true if so
    bool facingRight();

    //check if orentation is upright, returns true if so
    bool isUpright();

    //check if orentation is upside down, returns true if so
    bool isInverted();

    //check if orentation is flat, returns true if so
    bool isFlat();


private:
    I2C*    i2c;
    float SENSITIVITY;

    void deviceStandby();
    void deviceWake();
    void setRange(Range_setting const range);
    char readPLStat();
    void sendByteToRegister(char byte,char reg);
    char readByteFromRegister(char reg);
    void readBytesFromRegister(char reg,int numberOfBytes,char bytes[]);
};

#endif
