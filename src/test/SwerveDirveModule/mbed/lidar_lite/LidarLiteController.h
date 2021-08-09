#ifndef LidarLiteController_h
#define LidarLiteController_h

#include "mbed.h"
#include "LidarLite.h"

class LidarLiteController {
    public:
        /* Constructor for LidarLiteController. Initializes the three LidarLites.
         *  This constructor MUST be called! Otherwise, LidarLiteController's 
         *  static method calls will NOT WORK!
         */
        LidarLiteController();
        
        /* Calls LidarLite::getDistance() for all three LidarLites. If a 
         *  LidarLite fails to get the distance, this method attempts to
         *  reset the faulty LidarLite.
         *
         * @return: int* to distance measurements of the three LidarLites.
         */
        static int* getRawDistances();

        static int* getDistanceArr();
        
    private:
    
        /* Array of LidarLites.
         *  lidars[0]: left lidar
         *  lidars[1]: center lidar
         *  lidars[2]: right lidar
         */
        static LidarLite lidars[3];
        
        // holds the distances measured from each LidarLite
        static int rawDistanceArr[3];  
};

#endif