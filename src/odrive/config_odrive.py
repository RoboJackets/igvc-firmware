"""
Probably a better script detailed here:
https://github.com/AustinOwens/robodog/blob/main/odrive/configs/odrive_hoverboard_config.py
"""

import odrive
import time
from odrive.enums import *

odrv0 = odrive.find_any()
# odrv0.erase_configuration()
# time.sleep(3)

########################
### GENERAL SETTINGS ###
########################

# Problem: brake resistor saturation?
# isn't fixed when you switch odrives...
# maybe try and get AB encoder working for drive motors?

# odrv0.config.enable_brake_resistor = True
# odrv0.config.brake_resistance = 2
# odrv0.config.dc_max_negative_current = -0.010

#############################
### AXIS 0: Turning motor ###
#############################

# axis0 = getattr(odrv0, "axis{}".format(0))

# # axis.controller.config
# axis0.controller.config.vel_limit = 20

# # axis.motor.config
# axis0.motor.config.pole_pairs = 7
# axis0.motor.config.current_lim = 20
# axis0.motor.config.calibration_current = 10

# # axis.encoder.config
# axis0.encoder.config.cpr = 819
# axis0.encoder.config.mode = ENCODER_MODE_INCREMENTAL

# # Perform the calibration...
# axis0.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
# time.sleep(15)
# print("Calibrated axis0, the steering motor!")



# # Perform encoder offset calibration...
# axis0.encoder.config.use_index = True
# axis0.requested_state = AXIS_STATE_ENCODER_INDEX_SEARCH
# time.sleep(10)
# print("Found axis0's encoder index...")
# axis0.requested_state = AXIS_STATE_ENCODER_OFFSET_CALIBRATION
# time.sleep(20)
# print("Performed encoder offset calibration on axis0!")

#########################################
### AXIS 1: Drive motor, Hall Encoder ###
#########################################

axis1 = getattr(odrv0, "axis{}".format(1))

# axis.controller.config
axis1.controller.config.vel_limit = 20

# axis.motor.config
axis1.motor.config.pole_pairs = 10
axis1.motor.config.current_lim = 20
axis1.motor.config.calibration_current = 10

# axis.encoder.config
axis1.encoder.config.cpr = 60
axis1.encoder.config.mode = ENCODER_MODE_HALL

# Perform the calibration...
axis1.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
time.sleep(15)
print("Calibrated axis1, the drive motor!")

#######################################
### AXIS 1: Drive motor, AB Encoder ###
#######################################

# axis1 = getattr(odrv0, "axis{}".format(0))

# # axis.controller.config
# axis1.controller.config.vel_limit = 20

# # axis.motor.config
# axis1.motor.config.pole_pairs = 10
# axis1.motor.config.current_lim = 20
# axis1.motor.config.calibration_current = 10

# # axis.encoder.config
# axis1.encoder.config.cpr = 1024
# axis1.encoder.config.mode = ENCODER_MODE_INCREMENTAL

# # Perform the calibration...
# axis1.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
# time.sleep(15)
# print("Calibrated axis1, the drive motor!")




# odrv0.save_configuration()

# time.sleep(1)