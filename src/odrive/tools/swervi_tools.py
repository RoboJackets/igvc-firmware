import odrive
from odrive.enums import *
import sys
import time
# from fibre.protocol import ChannelBrokenException

class MotorConfig:

    def __init__(self):
        self._find_odrive()

    def _find_odrive(self):
        self.odrv = odrive.find_any()
        self.axis0 = getattr(self.odrv, "axis{}".format(0))
        self.axis1 = getattr(self.odrv, "axis{}".format(1))

    def configure_brake_resistor(self):
        print("Configuring the brake resistor...")
        self.odrv.config.enable_brake_resistor = True
        self.odrv.config.brake_resistance = 2
        self.odrv.config.dc_max_negative_current = -0.010

    def configure_axis0_motor(self):
        print("Configuring axis0, the steering motor...")
        # Change this to change direction of index search
        # self.axis0.config.calibration_lockin.accel = -20
        # self.axis0.config.calibration_lockin.vel = 40
        # self.axis0.config.calibration_lockin.ramp_distance = 3.14

        # self.axis0.encoder.config.direction = -1

        # axis.controller.config
        self.axis0.controller.config.vel_limit = 20
        self.axis0.controller.config.input_filter_bandwidth = 2.0
        self.axis0.controller.config.input_mode = INPUT_MODE_POS_FILTER

        # axis.motor.config
        self.axis0.motor.config.pole_pairs = 7
        self.axis0.motor.config.current_lim = 50
        self.axis0.motor.config.calibration_current = 20

        # axis.encoder.config
        self.axis0.encoder.config.cpr = 819
        self.axis0.encoder.config.mode = ENCODER_MODE_INCREMENTAL

        # self.axis0.encoder.config.find_idx_on_lockin_only = True

        # Perform the calibration...
        self.axis0.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
        print("Performing motor calibration for axis0, the steering motor...")
        input("When calibration completes, press ENTER.")

        print("Calibrated axis0, the steering motor!")

    def configure_axis0_index_signal(self):
        self.axis0.encoder.config.use_index = True
        self.axis0.requested_state = AXIS_STATE_ENCODER_INDEX_SEARCH
        print("Finding axis0's index signal...")
        input("When motor stops, press ENTER.")

    def configure_axis0_toggle_reverse_index_search(self, is_enabled):
        if is_enabled:
            self.axis0.config.calibration_lockin.accel = -20
        else:
            self.axis0.config.calibration_lockin.accel = 20

    def configure_axis0_encoder_offset_calibration(self):

        self.axis0.requested_state = AXIS_STATE_ENCODER_OFFSET_CALIBRATION
        print("Performing encoder offset calibration for axis0, the steering motor...")
        input("When calibration completes, press ENTER.")

    def configure_axis0_motor_pre_calibrated(self):
        self.axis0.motor.config.pre_calibrated = True

    def configure_axis0_pre_calibrated(self):
        print("axis0 done calibrating! Saving settings as pre-calibrated...")
        self.axis0.encoder.config.pre_calibrated = True
        self.axis0.config.startup_encoder_index_search = True
        self.axis0.motor.config.pre_calibrated = True

    def test_axis0(self):
        self.axis0.requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL

        positions = [0, 1, -1, 2, -2, 0]

        for position in positions:
            self.axis0.controller.input_pos = position
            time.sleep(5)

            if self.check_for_errors(0):
                return

    def test_axis1(self):
        self.axis1.requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL

        velocities = [0, 1, -1, 2, -2, 0]

        for velocity in velocities:
            self.axis1.controller.input_vel = velocity
            time.sleep(5)

            if self.check_for_errors(1):
                return


    def configure_axis1_pre_calibrated(self):
        print("axis0 done calibrating! Saving settings as pre-calibrated...")
        self.axis1.encoder.config.pre_calibrated = True
        self.axis1.motor.config.pre_calibrated = True

    def configure_axis1_motor(self):
        # axis.controller.config
        self.axis1.controller.config.vel_limit = 20
        self.axis1.controller.config.control_mode = CONTROL_MODE_VELOCITY_CONTROL

        # axis.motor.config
        self.axis1.motor.config.pole_pairs = 10
        self.axis1.motor.config.current_lim = 50
        self.axis1.motor.config.calibration_current = 20

        # axis.encoder.config
        self.axis1.encoder.config.cpr = 60
        self.axis1.encoder.config.mode = ENCODER_MODE_HALL

        # Perform the calibration...
        self.axis1.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE
        print("Performing motor calibration for axis1, the drive motor...")
        input("When calibration completes, press ENTER.")

    def save(self):
        print("Saving...")
        try:
            self.odrv.save_configuration()
        except:
            pass

        self._find_odrive()

    def erase(self):
        print("Erasing...")
        try:
            self.odrv.erase_configuration()
        except:
            pass

        self._find_odrive()

    def reboot(self):
        print("Rebooting...")
        try:
            self.odrv.reboot()
        except:
            pass

        self._find_odrive()

    def clear_errors(self):
        print("Clearing errors...")
        self.odrv.clear_errors()

    def check_for_errors(self, axis):

        error_found = False

        print("Checking for errors...")

        if axis == 0:
            if self.axis0.motor.error != 0:
                print("Error: Odrive reported a motor error of {} for axis0."
                      " Printing out Odrive motor data for "
                      "debug:\n{}".format(self.axis0.motor.error,
                                          self.axis0.motor))
                error_found = True

            if self.axis0.encoder.error != 0:
                print("Error: Odrive reported an encoder error of {} for axis0."
                      " Printing out Odrive motor data for "
                      "debug:\n{}".format(self.axis0.motor.error,
                                          self.axis0.motor))

                error_found = True


        elif axis == 1:
            if self.axis1.motor.error != 0:
                print("Error: Odrive reported a motor error of {} for axis1."
                      " Printing out Odrive motor data for "
                      "debug:\n{}".format(self.axis1.motor.error,
                                          self.axis1.motor))
                error_found = True

            if self.axis1.encoder.error != 0:
                print("Error: Odrive reported an encoder error of {} for axis1."
                      " Printing out Odrive motor data for "
                      "debug:\n{}".format(self.axis1.motor.error,
                                          self.axis1.motor))
                error_found = True


        if error_found:
            return True
        else:
            print("No errors found!")
            return False

def new_full_calib_sequence(motor):
    motor.erase()

    print("BEGIN SWERVE DRIVE MODULE CALIBRATION")
    print("CONFIGURING AXIS 0: TURNING MOTOR")
    input("Ensure that the hub motor has been dropped SWERVI. Then, press ENTER...")

    # STEP 1: enable the brake resistor
    # Save + Reboot to ensure settings are applied
    motor.configure_brake_resistor()
    motor.save()
    motor.reboot()

    # STEP 2: Do initial motor calibration
    # Its OK if an error occurs, we just want the motor to move
    # for finding the index signal
    # motor.configure_axis0_motor()
    # motor.clear_errors()

    while True:
        motor.configure_axis0_motor()
        errors_occurred = motor.check_for_errors(0)

        if errors_occurred:
            print("Motor calibration failed, retrying...")
            motor.save()
            motor.reboot()
            continue
        else:
            break

    motor.configure_axis0_motor_pre_calibrated()
    motor.save()
    motor.reboot()

    while True:

        motor.configure_axis0_index_signal()

        # motor.configure_axis0_toggle_reverse_index_search(True)

        motor.configure_axis0_encoder_offset_calibration()
        errors_occurred = motor.check_for_errors(0)

        if errors_occurred:
            print("Encoder offset calibration failed, retrying...")
            # motor.configure_axis0_toggle_reverse_index_search(False)
            motor.save()
            motor.reboot()
            continue
        else:
            break

    motor.configure_axis0_pre_calibrated()
    motor.save()
    motor.reboot()

    # STEP 6: Calibrate the drive motor
    print("CONFIGURING AXIS 1: DRIVE MOTOR")
    input("Fit the hub motor back onto SWERVI. "
          "Alternatively, you can also hold the hub motor upright so the wheel"
          "can spin freely.\n Then, press ENTER...")

    while True:
        motor.configure_axis1_motor()
        errors_occurred = motor.check_for_errors(1)

        if not errors_occurred:
            break
        else:
            print("Motor calibration failed, retrying...")
            motor.save()
            motor.reboot()

    # STEP 7: Set the motor and encoder to pre-calibrated
    print("All calibration complete. Put the hub motor back onto SWERVI.")
    input("Then, press ENTER.")
    motor.configure_axis1_pre_calibrated()
    motor.save()
    motor.reboot()

def full_calib_sequence(motor):
    motor.erase()

    print("BEGIN SWERVE DRIVE MODULE CALIBRATION")
    print("CONFIGURING AXIS 0: TURNING MOTOR")
    input("Ensure that the hub motor has been dropped SWERVI. Then, press ENTER...")

    # STEP 1: enable the brake resistor
    # Save + Reboot to ensure settings are applied
    motor.configure_brake_resistor()
    motor.save()
    motor.reboot()

    # STEP 2: Do initial motor calibration
    # Its OK if an error occurs, we just want the motor to move
    # for finding the index signal
    motor.configure_axis0_motor()
    motor.clear_errors()

    # STEP 3: Find the index signal
    motor.configure_axis0_index_signal()

    # STEP 4: Re-calibrate the motor
    # Calibration must pass. Try calibrating until it passes
    while True:
        motor.configure_axis0_motor()
        errors_occurred = motor.check_for_errors(0)

        if errors_occurred:
            print("Motor calibration failed, retrying...")
            motor.save()
            motor.reboot()
            continue

        # STEP 5: Perform offset calibration for the encoder
        motor.configure_axis0_encoder_offset_calibration()
        errors_occurred = motor.check_for_errors(0)

        if errors_occurred:
            print("Encoder offset calibration failed, retrying...")
            motor.save()
            motor.reboot()
            continue
        else:
            break

    motor.configure_axis0_pre_calibrated()
    motor.save()
    motor.reboot()

    # STEP 6: Calibrate the drive motor
    print("CONFIGURING AXIS 1: DRIVE MOTOR")
    input("Fit the hub motor back onto SWERVI. "
          "Alternatively, you can also hold the hub motor upright so the wheel"
          "can spin freely.\n Then, press ENTER...")

    while True:
        motor.configure_axis1_motor()
        errors_occurred = motor.check_for_errors(1)

        if not errors_occurred:
            break
        else:
            print("Motor calibration failed, retrying...")
            motor.save()
            motor.reboot()

    # STEP 7: Set the motor and encoder to pre-calibrated
    print("All calibration complete. Put the hub motor back onto SWERVI.")
    input("Then, press ENTER.")
    motor.configure_axis1_pre_calibrated()
    motor.save()
    motor.reboot()

def full_testing_sequence(motor):

    motor.reboot()

    print("Testing axis0, the turning motor.")
    input("Press ENTER when ready.")
    motor.test_axis0()
    print("Testing axis1, the driving motor.")
    input("Press ENTER when ready.")
    motor.test_axis1()

    motor.reboot()

    print("Testing Complete.")

def _steering_calib(motor, useReversed):

    motor.erase()

    print("BEGIN SWERVE DRIVE MODULE CALIBRATION")
    print("CONFIGURING AXIS 0: TURNING MOTOR")
    input("Ensure that the hub motor has been dropped SWERVI. Then, press ENTER...")

    # STEP 1: enable the brake resistor
    # Save + Reboot to ensure settings are applied
    motor.configure_brake_resistor()
    motor.save()
    motor.reboot()

    # STEP 2: Do initial motor calibration
    # Its OK if an error occurs, we just want the motor to move
    # for finding the index signal
    # motor.configure_axis0_motor()
    # motor.clear_errors()

    while True:
        motor.configure_axis0_motor()
        errors_occurred = motor.check_for_errors(0)

        if errors_occurred:
            print("Motor calibration failed, retrying...")
            motor.save()
            motor.reboot()
            continue
        else:
            break

    motor.configure_axis0_motor_pre_calibrated()
    motor.save()
    motor.reboot()

    while True:

        motor.configure_axis0_index_signal()

        # motor.configure_axis0_toggle_reverse_index_search(True)

        motor.configure_axis0_encoder_offset_calibration()
        errors_occurred = motor.check_for_errors(0)

        if errors_occurred:
            print("Encoder offset calibration failed, retrying...")
            # motor.configure_axis0_toggle_reverse_index_search(False)
            motor.save()
            motor.reboot()
            continue
        else:
            break

    motor.configure_axis0_pre_calibrated()
    motor.save()
    motor.reboot()

    print("Testing axis0, the turning motor.")
    input("Press ENTER when ready. Alternatively, press CTRL + C to quit.")
    motor.test_axis0()

def _drive_calib(motor):
    # STEP 6: Calibrate the drive motor
    print("CONFIGURING AXIS 1: DRIVE MOTOR")
    input("Fit the hub motor back onto SWERVI. "
          "Alternatively, you can also hold the hub motor upright so the wheel"
          "can spin freely.\n Then, press ENTER...")

    while True:
        motor.configure_axis1_motor()
        errors_occurred = motor.check_for_errors(1)

        if not errors_occurred:
            break
        else:
            print("Motor calibration failed, retrying...")
            motor.save()
            motor.reboot()

    # STEP 7: Set the motor and encoder to pre-calibrated
    print("All calibration complete. Put the hub motor back onto SWERVI.")
    input("Then, press ENTER.")
    motor.configure_axis1_pre_calibrated()
    motor.save()
    motor.reboot()

    print("Testing axis1, the driving motor.")
    input("Press ENTER when ready. Alternatively, press CTRL + C to quit.")
    motor.test_axis1()

# ORANGE MOTOR
def orange_steering_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)

def orange_drive_calib():
    motor = MotorConfig()
    _drive_calib(motor)

def orange_full_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)
    _drive_calib(motor)

# WHITE MOTOR
def white_steering_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)

def white_drive_calib():
    motor = MotorConfig()
    _drive_calib(motor)

def white_full_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)
    _drive_calib(motor)

# RED MOTOR
def red_steering_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)

def red_drive_calib():
    motor = MotorConfig()
    _drive_calib(motor)

def red_full_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)
    _drive_calib(motor)

# BROWN MOTOR
def brown_steering_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)

def brown_drive_calib():
    motor = MotorConfig()
    _drive_calib(motor)

def brown_full_calib():
    motor = MotorConfig()
    _steering_calib(motor, True)
    _drive_calib(motor)


# if __name__== "__main__":

#     motor = MotorConfig()

#     # full_calib_sequence(motor)

#     # new_full_calib_sequence(motor)
#     full_testing_sequence(motor)
