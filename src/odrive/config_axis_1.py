"""
Probably a better script detailed here:
https://github.com/AustinOwens/robodog/blob/main/odrive/configs/odrive_hoverboard_config.py
"""

import odrive
import time
from odrive.enums import *

odrv0 = odrive.find_any()

#########################################
### AXIS 1: Drive motor, Hall Encoder ###
#########################################

axis1 = getattr(odrv0, "axis{}".format(1))

# axis.controller.config
axis1.controller.config.vel_limit = 20

# axis.motor.config
axis1.motor.config.pole_pairs = 10
axis1.motor.config.current_lim = 50
axis1.motor.config.calibration_current = 20

# axis.encoder.config
axis1.encoder.config.cpr = 60
axis1.encoder.config.mode = ENCODER_MODE_HALL

axis1.controller.config.control_mode = CONTROL_MODE_VELOCITY_CONTROL

# Perform the calibration...
axis1.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
time.sleep(15)
print("Calibrated axis1, the drive motor!")

##########################
### SAVE CONFIGURATION ###
##########################

# odrv0.save_configuration()