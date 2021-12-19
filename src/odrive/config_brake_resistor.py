"""
Probably a better script detailed here:
https://github.com/AustinOwens/robodog/blob/main/odrive/configs/odrive_hoverboard_config.py
"""

import odrive
import time
from odrive.enums import *

odrv0 = odrive.find_any()

########################
### GENERAL SETTINGS ###
########################

# Problem: brake resistor saturation?
# isn't fixed when you switch odrives...
# maybe try and get AB encoder working for drive motors?

odrv0.config.enable_brake_resistor = True
odrv0.config.brake_resistance = 2
odrv0.config.dc_max_negative_current = -0.010
# odrv0.config.max_regen_current = 10

##########################
### SAVE CONFIGURATION ###
##########################

odrv0.save_configuration()