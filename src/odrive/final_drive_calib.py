import odrive
from odrive.enums import *
import sys
import time
# from fibre.protocol import ChannelBrokenException

FL_ANGLE_CAN_ID = 0X3
FL_VEL_CAN_ID   = 0X5
FR_ANGLE_CAN_ID = 0X13
FR_VEL_CAN_ID   = 0X15
BL_ANGLE_CAN_ID = 0X33
BL_VEL_CAN_ID   = 0X35
BR_ANGLE_CAN_ID = 0X23
BR_VEL_CAN_ID   = 0X25

def save_config(axisNum):
    print("Saving...")
    odrv, _ = find_odrv_axis(axisNum)

    try:
        odrv.save_configuration()
    except:
        pass

    odrv, _ = find_odrv_axis(axisNum)

    try:
        odrv.reboot()
    except:
        pass  

    time.sleep(1)

    return find_odrv_axis(axisNum)

def erase_config():
    # erase the odrive
    print("Erasing...")
    odrv, _ = find_odrv_axis(0)

    try:
        odrv.erase_configuration()
    except:
        pass
    
    return find_odrv_axis(0)

def find_odrv_axis(axisNum):

    odrv = odrive.find_any()
    axis = getattr(odrv, "axis{}".format(axisNum))

    return (odrv, axis)

def check_for_errors(axisNum):

    _, axis = find_odrv_axis(axisNum)

    error_found = False

    print("Checking for errors...")

    if axis.motor.error != 0:
        print("Error: Odrive reported a motor error of {} for axis0."
                " Printing out Odrive motor data for "
                "debug:\n{}".format(axis.motor.error,
                                    axis.motor))
        error_found = True

    if axis.encoder.error != 0:
        print("Error: Odrive reported an encoder error of {} for axis0."
                " Printing out Odrive motor data for "
                "debug:\n{}".format(axis.motor.error,
                                    axis.motor))

        error_found = True


    if error_found:
        return True
    else:
        print("No errors found!")
        return False

def configure_axis_pre_calibrated(axisNum):
    print("axis done calibrating! Saving settings as pre-calibrated...")

    _, axis = find_odrv_axis(axisNum)

    axis.encoder.config.pre_calibrated = True
    axis.motor.config.pre_calibrated = True

    
def calib_axis(axisNum):
    # find the odrive
    odrv, axis = find_odrv_axis(axisNum)

    # configure brake resistor
    print("Configuring the brake resistor...")
    odrv.config.enable_brake_resistor = True
    odrv.config.brake_resistance = 2
    odrv.config.dc_max_negative_current = -0.010

    # save
    odrv, axis = save_config(axisNum)

    # calibrate the drive motor
    print("CONFIGURING AXIS 1: DRIVE MOTOR")
    input("Fit the hub motor back onto SWERVI. "
          "Alternatively, you can also hold the hub motor upright so the wheel"
          "can spin freely.\n Then, press ENTER...")

    while True:

        # NEWr
        # odrv.config
        # odrv.config.dc_max_negative_current = -0.1
        # odrv.config.max_regen_current = 5

        # axis.controller.config
        axis.controller.config.vel_limit = 20
        axis.controller.config.control_mode = CONTROL_MODE_VELOCITY_CONTROL

        # axis.motor.config
        axis.motor.config.pole_pairs = 10
        axis.motor.config.current_lim = 20
        axis.motor.config.calibration_current = 10

        # axis.encoder.config
        axis.encoder.config.cpr = 60
        axis.encoder.config.mode = ENCODER_MODE_HALL

        # Perform the calibration...
        axis.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
        print("Performing motor calibration for axis1, the drive motor...")
        input("When calibration completes, press ENTER.")

        errors_occurred = check_for_errors(axisNum)

        if not errors_occurred:
            break
        else:
            print("Motor calibration failed, retrying...")
            odrv, axis = save_config(axisNum)

    # configure settings as precalibrated
    print("All calibration complete. Put the hub motor back onto SWERVI.")
    input("Then, press ENTER.")
    configure_axis_pre_calibrated(axisNum)

    # set CAN address
    print("Setting the CAN addresses")
    odrv.can.config.baud_rate = 500000

    while True:
        inputStr1 = input('Type "back" if motor is behind the LiDAR. Type "front" if motor is in front of the LiDAR. ')
        inputStr2 = input('Standing behind the robot, type "left" or "right". ')

        if (inputStr1 == "back" and inputStr2 == "left"):
            axis.config.can.node_id = BL_VEL_CAN_ID
        elif (inputStr1 == "back" and inputStr2 == "right"):
            axis.config.can.node_id = BR_VEL_CAN_ID
        elif (inputStr1 == "front" and inputStr2 == "left"):
            axis.config.can.node_id = FL_VEL_CAN_ID
        elif (inputStr1 == "front" and inputStr2 == "right"):
            axis.config.can.node_id = FR_VEL_CAN_ID
        else:
            print("Incorrect input.")
            continue

        break

    # save everything
    save_config(axisNum)

def print_help():
    print("Usage: python3 final_drive_calib.py [axisNum]")
    print("[axisNum]: either 0 or 1")

if __name__ == "__main__":

    argNum = len(sys.argv)

    if argNum != 2:
        print("Incorrect number of arguments")
        print_help()
    else:  
        arg = sys.argv[1]

        if arg == "erase":
            erase_config()
        elif arg != "0" and arg != "1":
            print("Illegal Argument")
            print_help()
        else: 
            calib_axis(arg)