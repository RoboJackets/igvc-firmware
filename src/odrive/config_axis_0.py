"""
Probably a better script detailed here:
https://github.com/AustinOwens/robodog/blob/main/odrive/configs/odrive_hoverboard_config.py
"""

import odrive
import time
from odrive.enums import *

odrv0 = odrive.find_any()

#############################
### AXIS 0: Turning motor ###
#############################

axis0 = getattr(odrv0, "axis{}".format(0))

# Change this to change direction of index search
axis0.config.calibration_lockin.accel = 20

# axis.controller.config
axis0.controller.config.vel_limit = 20
axis0.controller.config.input_filter_bandwidth = 2.0
axis0.controller.config.input_mode = INPUT_MODE_POS_FILTER

# axis.motor.config
axis0.motor.config.pole_pairs = 7
axis0.motor.config.current_lim = 50
axis0.motor.config.calibration_current = 20

# axis.encoder.config
axis0.encoder.config.cpr = 819
axis0.encoder.config.mode = ENCODER_MODE_INCREMENTAL

# Perform the calibration...
axis0.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
time.sleep(20)
print("Calibrated axis0, the steering motor!")
#
# # Perform encoder offset calibration...
# axis0.encoder.config.use_index = True
#
# axis0.requested_state = AXIS_STATE_ENCODER_INDEX_SEARCH
# time.sleep(20)
# print("Found axis0's encoder index...")
#
# axis0.requested_state = AXIS_STATE_ENCODER_OFFSET_CALIBRATION
# time.sleep(20)
# print("Performed encoder offset calibration on axis0!")
#
#
#
#
# # # Set encoder and motor to precalibrated
# axis0.encoder.config.pre_calibrated = True
# axis0.config.startup_encoder_index_search = True
# axis0.motor.config.pre_calibrated = True



##########################
### SAVE CONFIGURATION ###
##########################

# odrv0.save_configuration()