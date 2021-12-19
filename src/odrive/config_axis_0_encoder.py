"""
Probably a better script detailed here:
https://github.com/AustinOwens/robodog/blob/main/odrive/configs/odrive_hoverboard_config.py
"""

import odrive
import time
import math
from odrive.enums import *

odrv0 = odrive.find_any()

#############################
### AXIS 0: Turning motor ###
#############################

axis0 = getattr(odrv0, "axis{}".format(0))

#Perform encoder offset calibration...

# odrv0.axis0.config.calibration_lockin.vel = -40
# odrv0.axis0.config.calibration_lockin.accel = -20
# odrv0.axis0.config.calibration_lockin.ramp_distance = -1 * math.pi

# axis0.encoder.config.use_index = True
# axis0.requested_state = AXIS_STATE_ENCODER_INDEX_SEARCH
# time.sleep(10)
# print("Found axis0's encoder index...")

axis0.requested_state = AXIS_STATE_ENCODER_OFFSET_CALIBRATION
time.sleep(20)
print("Performed encoder offset calibration on axis0!")
#
axis0.encoder.config.pre_calibrated = True
axis0.config.startup_encoder_index_search = True
axis0.motor.config.pre_calibrated = True


##########################
### SAVE CONFIGURATION ###
##########################

# odrv0.save_configuration()