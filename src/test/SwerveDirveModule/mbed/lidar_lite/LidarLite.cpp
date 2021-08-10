#include "LidarLite.h"
#include "LidarLiteController.h"
#include "mbed.h"
#include <string>

PinName LidarLite::sdaPin(p9);
PinName LidarLite::sclPin(p10);
I2C LidarLite::i2c(sdaPin, sclPin);

LidarLite::LidarLite() {
             
    lidarliteAddr = LIDARLITE_ADDR_DEFAULT;
    measurementsNoBiasCorrection = 0;                       
}

//NOT USED!
LidarLite::LidarLite(char _lidarliteAddress = LIDARLITE_ADDR_DEFAULT, 
                     PinName _enablePinName = NC) {
    
    lidarliteAddr = _lidarliteAddress;
    measurementsNoBiasCorrection = 0;
    
    enable();
    
    //change I2C address to something diffenent than the default address
    changeI2CAddress(LIDARLITE_ADDR_DEFAULT, lidarliteAddr);
    
    //write configuration values to the LidarLite
    setLidarSettings();
}

//!!! LOOK AT DOCUMENTATION FOR DESCRIPTION OF CONFIG VALUES !!!
bool LidarLite::setLidarSettings() {
    
    char dataArr[1];
    bool i2cSuccessful = true;
    
    dataArr[0] = 0x80;
    i2cSuccessful &= write(0x02, dataArr, 1, lidarliteAddr);

    dataArr[0] = 0x08;
    i2cSuccessful &= write(0x04, dataArr, 1, lidarliteAddr);

    dataArr[0] = 0x00;
    i2cSuccessful &= write(0x1c, dataArr, 1, lidarliteAddr);
    
    if (i2cSuccessful) {
        return true;
    } else {
        return false;
    }
}

void LidarLite::setEnablePin(PinName _enablePin) {
    
    enablePin = _enablePin;
    disable();
}

bool LidarLite::setupLidar(char _lidarliteAddress,
                            std::string name) {
    
    lidarliteAddr = _lidarliteAddress;
    lidarliteName = name;
    bool i2cSuccessful = true;
    
    enable();
    
    i2cSuccessful &= changeI2CAddress(LIDARLITE_ADDR_DEFAULT, lidarliteAddr);    
    i2cSuccessful &= setLidarSettings();
    
    return i2cSuccessful;
}

bool LidarLite::reset() {
    
    bool i2cSuccessful = true;
    
    disable();
    i2cSuccessful &= setupLidar(lidarliteAddr, lidarliteName);
    
    return i2cSuccessful;
}

void LidarLite::enable() {
    DigitalOut(enablePin, 1);
}

void LidarLite::disable() {
    DigitalOut(enablePin, 0);
}

std::string LidarLite::getName() {
    return lidarliteName;   
}

int LidarLite::getDistance() {
    
    char dataArr[1];
    bool i2cSuccessful = true;

    /* If we have taken 100 consecutive distance measurements, enable bias
     *  correction.
     * Otherwise, prepare a normal distance measurement.
     */
    if(measurementsNoBiasCorrection < 1) {
        dataArr[0] = 0x03;
        measurementsNoBiasCorrection++;          
    } else {
        dataArr[0] = 0x04;
        measurementsNoBiasCorrection = 0;  
    }

    // Take a distance measurement.
    i2cSuccessful &= write(0x00, dataArr, 1, lidarliteAddr);

    // Read the distance measurement for the lidar.
    char distanceArray[2];
    i2cSuccessful &= read(0x8f, distanceArray, 2, lidarliteAddr);
    
    /* If a read/write fails, return -1.
     *  Otherwise, return the distance.
     */
    if (i2cSuccessful == true) {
        int distance = (distanceArray[0] << 8) + distanceArray[1];
        return distance;
    } else {
        return -1;
    }
}

char LidarLite::readI2CAddress(char currentLidarliteAddress) {
    
    bool i2cSuccessful = true;
    char dataArr[2];
    
    // Read the unique serial number from the LidarLite
    i2cSuccessful &= read(0x96, dataArr, 2, currentLidarliteAddress);
    /* Write the unique serial number back to the LidarLite
     *  This unlocks the LidarLite.
     */
    i2cSuccessful &= write(0x98, dataArr, 2, currentLidarliteAddress);
    
    // read the new I2C address to the LidarLite. 
    dataArr[0] = 0x69;
    i2cSuccessful &= read(0x1a, dataArr, 1, currentLidarliteAddress);
    
    return dataArr[0];
    
    // Enable the new I2C address on LidarLite.
    //i2cSuccessful &= read(0x1e, dataArr, 1, currentLidarliteAddress);
    //dataArr[0] = dataArr[0] | (1 << 4);
    //i2cSuccessful &= write(0x1e, dataArr, 1, currentLidarliteAddress);
}

bool LidarLite::changeI2CAddress(char currentLidarliteAddress, char newI2CAddress) {
    
    bool i2cSuccessful = true;
    char dataArr[2];
    
    // Read the unique serial number from the LidarLite
    i2cSuccessful &= read(0x96, dataArr, 2, currentLidarliteAddress);
    /* Write the unique serial number back to the LidarLite
     *  This unlocks the LidarLite.
     */
    i2cSuccessful &= write(0x98, dataArr, 2, currentLidarliteAddress);
    
    // Write the new I2C address to the LidarLite. 
    dataArr[0] = (newI2CAddress << 1);
    i2cSuccessful &= write(0x1a, dataArr, 1, currentLidarliteAddress);
    
    /* Disables the default address. If the default address is not disabled, 
     *  then attempts to change the I2C address of an unitialized LidarLite
     *  will change the I2C address of the first LidarLite.
     *
     * The documentation is incorrect; writing "0x08" to the LidarLite does 
     *  not work. Instead,
     *  (1) Write "0x10" to keep the default address enabled
     *  (2) Write "0x18" to disable the default address
     *
     *  The LidarLite::write() function always returns false for some reason.
     */
     
    dataArr[0] = 0x18;
    write(0x1e, dataArr, 1, currentLidarliteAddress);
        
    /* If a read/write failed, returned true.
     *  Otherwise, return false.
     */
    if (i2cSuccessful) {
        return true;
    } else {
        return false;
    }
}

bool LidarLite::write(char registerAddr, char* dataArr, 
                        int numBytes, char lidarliteAddress) {
    /* I2C read/write uses 8 bit I2C address.
     *  Obtain it by left shifting the bits of the 7 bit I2C address.
     */
    char lidarliteAddress_8bit = lidarliteAddress << 1;
    
    bool isDisconnected = false;
    int nack = 1;
    int delayCount = 0;
    
    /* i2cData holds all of the data the LidarLite needs to process a write 
     *  command. 
     *  i2cData[0] = registerAddr, address in LidarLite being written to
     *  i2cData[1-2] = dataArr, data being written to the LidarLite
     */
    char i2cData[1 + numBytes];

    for (int i = 0; i < 1 + numBytes; i++) {
        if (i == 0) {
            i2cData[i] = registerAddr;
        } else {
            i2cData[i] = dataArr[i - 1];
        }
    }

    /* Attempts an I2C::write
     *  stops after 5 attempts to avoid being stuck in an infinite loop
     */
    while (nack != 0 && !isDisconnected) {
        // i2c.write() returns 0 if write command succeeds
        nack = i2c.write(lidarliteAddress_8bit, i2cData, 1 + numBytes);
        
        delayCount++;
        // ThisThread::sleep_for(1);
        
        if (delayCount >= 5) {
            isDisconnected = true;   
        }
    }
    
    /* If I2C::write succeeded, return true.
     *  If I2C::write timed out, return false.
     */
    if (!isDisconnected) {
        return true;
    } else {
        return false;
    }
}

bool LidarLite::read(char registerAddr, char *dataArr, 
                        int numBytes, char lidarliteAddress) {

    /* I2C read/write uses 8 bit I2C address.
     *  Obtain it by left shifting the bits of the 7 bit I2C address.
     */
    char lidarliteAddress_8bit = lidarliteAddress << 1;
    
    bool isDisconnected = false;
    int delayCount = 0;
    int nack = 1;
    
    /* Attempts an I2C::write
     *  stops after 5 attempts to avoid being stuck in an infinite loop
     */
    while (nack != 0 && !isDisconnected) {
        // i2c.write() returns 0 if write command succeeds
        nack = i2c.write(lidarliteAddress_8bit, &registerAddr, 1);  
        //nack = i2c.write(addr8, &registerAddr, 1); 
        
        delayCount++;
        // ThisThread::sleep_for(1);
        
        if (delayCount >= 5) {
            isDisconnected = true; 
        }
    }
    
    delayCount = 0;
    nack = 1;
    
    /* Attempts an I2C::read
     *  stops after 5 attempts to avoid being stuck in an infinite loop
     */
    while (nack != 0 && !isDisconnected) {
        /* Data read from the LidarLite is stored into dataArr.
         *
         * i2c.read() returns 0 if read command succeeds
         */
        nack = i2c.read(lidarliteAddress_8bit, dataArr, numBytes);  
        
        delayCount++;
        // ThisThread::sleep_for(1);
        
        if (delayCount >= 5) {
            isDisconnected = true;
            break;
        } 
    }
    
    /* If I2C::write and I2C::read succeeded, return true.
     *  If either I2C::write or I2C::read timed out, return false.
     */
    if (!isDisconnected) {
        return true; 
    } else {
        return false;
    }
}