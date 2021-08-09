#ifndef LidarLite_h
#define LidarLite_h

#define LIDARLITE_ADDR_DEFAULT 0x62

#include "mbed.h"
#include <string>

/* LidarLite Wire Quick Reference:
 *  RED: 5V
 *  BLACK: GND
 *  GREEN: SCL (I2C)
 *  BLUE: SDA (I2C)
 *  ORANGE: Power Enable
 *  YELLOW: Mode Control (NOT USED)
 *
 * LidarLite v3 Documentation
 *  https://cdn.sparkfun.com/assets/f/e/6/3/7/PM-14032.pdf
 *
 */
class LidarLite {
    public:
        /* Default constructor for a single LidarLite object.
         *  Used for making array of LidarLites in LidarLiteController class.
         *  New LidarLites MUST call setupLidar().
         */
        LidarLite();
        
        /* NOT USED (still doesn't set enablePin)
         *  Constructor that fully sets up LidarLite object.
         *
         * @param _lidarliteAddress: I2C address of the LidarLite
         * @param _enablePin: PinName of the power enable pin associated
         *                      with this LidarLite
         */
        LidarLite(char _lidarliteAddress, PinName _enablePin);
        
        /* Resets the LidarLite. This is called automatically when the mbed
         *  loses communication with the LidarLite and/or the LidarLite loses 
         *  power.
         *
         * @return: true if mbed re-establishes communication with the LidarLite,
         *         false if mbed cannot reconnect.
         */
        bool reset();
        
        //!!! Idea: make these return bools, check if PinName is valid
        
        /* Enables the LidarLite using the power enable pin. 
         *  NOTE: enablePin variable MUST be initialized!
         */
        void enable();
        
        /* Disables the LidarLite using the power enable pin.
         *  NOTE: enablePin variable MUST be initialized!
         *  IMPORTANT: if a LidarLite is disabled, it DOES NOT remember what 
         *      its previous custom I2C address was!
         */
        void disable();
        
        /* Returns the name of the LidarLite
         * 
         * @return name of the LidarLite
         */
        std::string getName();
        
        /* LidarLite takes a distance measurement. 
         *
         * @return distance in front of LidarLite in centimeters
         */
        int getDistance();
        
        /* Changes the address of the LidarLite.
         * 
         * @param currentLidarAddress: current I2C address of the LidarLite
         *                              default 0x62
         * @param newI2CAddress: new I2C address of the LidarLite
         * @return: true if I2C address was successfully changed, false
         *           false if not
         */
        bool changeI2CAddress(char currentLidarliteAddress, char newI2CAddress); 
        
        /* Sets up the enable pin for the LidarLite. Must be called before 
         *  setEnablePin().
         *
         * @param _enablePin: PinName of the power enable pin for this LidarLite
         */
        void setEnablePin(PinName _enablePin);
        
        /* Sets up a LidarLite object after it is constructed using the 
         *  default constructor.
         *
         * @param _lidarliteAddress: I2C address of the LidarLite
         * @param name: name of the LidarLite
         *
         * @return: true if the setup is successful, false otherwise
         */
        bool setupLidar(char _lidarliteAddress, const std::string name);
        
        /* mbed writes configuration values to the LidarLite. Used in
         *  setupLidar().
         */
        bool setLidarSettings();
        
        /* Write to the LidarLite using I2C.
         *
         * @param registerAddr: memory address in the LidarLite to write to
         * @param *dataArr: character array, transports bytes into I2C::write()
         * @param numBytes: number of bytes in dataArr
         * @param lidarLiteAddress: 7 bit address of the LidarLite (e.g. 0x12)
         *
         * @return: true if write was successful, false if write fails
         */
        bool write (char registerAddr, char* dataArr, int numBytes, char lidarliteAddress);
        
        /* Read from the LidarLite using I2C. The data read from the
         *  LidarLite is stored into dataArr.
         * 
         * @param registerAddr: memory address in the LidarLite to read from
         * @param *dataArr: character array, transports bytes into I2C::read()
         *                   and I2C::write(). Also stores the data read from 
         *                   the LidarLite.
         * @param numBytes: number of bytes in dataArr
         * @param lidarLiteAddress: 7 bit address of the LidarLite (e.g. 0x12)
         * 
         * @return: true if read was successful, false read fails
         */
        bool read(char registerAddr, char *dataArr, int numBytes, char lidarliteAddress);
        
    private:
        // I2C Class Variables for LidarLite used for I2C communication
        static PinName sdaPin;
        static PinName sclPin;
        static I2C i2c;
        
        /* Used for counting the number times getDistance() is called.
         *  Once this value reaches 100, the LidarLite runs a bias correction.
         */
        int measurementsNoBiasCorrection;
        
        //Address of the LidarLite
        char lidarliteAddr;
        
        /* Enable pin on the mbed for the LidarLidar.
         *  Used when resetting a lidar.
         */
        PinName enablePin;
        
        //name of the LidarLite
        std::string lidarliteName;
        
        
        char readI2CAddress(char currentLidarliteAddress);
};

#endif