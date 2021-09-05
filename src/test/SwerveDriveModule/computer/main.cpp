#include <stdio.h>
#include <iostream>
#include <string.h>
#include "communication/ParseProtobuf.h"

#define MAX_ODRIVE 1
#define MAX_AXIS 1

using namespace std;

void help_message();
void error_message();
void print_send_data(int can_id, int axis_id, int cmd_id);
void print_send_data(int can_id, int axis_id, int cmd_id, int val);
void print_send_data(int can_id, int axis_id, int cmd_id, float val);

ParseProtobuf eth;

int main() {

    ParseProtobuf eth = ParseProtobuf(); 
    eth.connect();
    
    help_message();

    char input[30];
    char delim[] = " ";
    char *ptr;

    int num_args;
    char cmd[15];
    char cmd_value[15];

    int can_id = 0;
    int axis_id = 0; 

    while (true) {
        printf(" > ");
        cin.getline(input, 30);

        ptr = strtok(input, delim);
        memcpy(cmd, ptr, 15);
        num_args = 1;
        // printf("%s\n", cmd);
        ptr = strtok(nullptr, delim);
        if (ptr != nullptr) {
            memcpy(cmd_value, ptr, 15);
            num_args = 2;
            // printf("%s\n", cmd_value);
        }
            
        if (strcmp(cmd, "odrive") == 0 && num_args == 2) {
            int val; 
            sscanf(cmd_value, "%d", &val);
            
            if (val >= 0 && val <= MAX_ODRIVE) {
                can_id = val;
                printf("Switched to odrive %d.\n", val);
            } else {
                printf("Invalid command!\n");
                printf("Type 'help' for more information.\n");
            }
        }  else if (strcmp(cmd, "axis") == 0 && num_args == 2) {
            int val; 
            sscanf(cmd_value, "%d", &val);
            
            if (val >= 0 && val <= MAX_AXIS) {
                axis_id = val;
                printf("Switched to axis %d on odrive %d.\n", 
                    axis_id, can_id);
            } else {
                printf("Invalid command!\n");
                printf("Type 'help' for more information.\n");
            }
        } else if (strcmp(cmd, "axis_state") == 0 && num_args == 2) {
            
            int val = -1;

            if (strcmp(cmd_value, "CALIB") == 0) {
                val = 3;
            } else if (strcmp(cmd_value, "IDLE") == 0) {
                val = 1;
            } else if (strcmp(cmd_value, "CONTROL") == 0) {
                val = 8;
            } else {
                error_message();
            }

            if (val != -1) {
                print_send_data(can_id, axis_id, 0x07, val);
                // eth.send_message(can_id, axis_id, 
                //     0x07, static_cast<uint32_t>(val));
            }
           
        } else if (strcmp(cmd, "control_mode") == 0 && num_args == 2) {
            
            int val = -1;

            if (strcmp(cmd_value, "POS") == 0) {
                val = 3;
            } else if (strcmp(cmd_value, "VEL") == 0) {
                val = 2;
            } else {
                error_message();
            }

            if (val != -1) {
                print_send_data(can_id, axis_id, 0x0B, val);
                // eth.send_message(can_id, axis_id, 
                //     0x07, static_cast<uint32_t>(val));
            }
           
        } else if (strcmp(cmd, "pos") == 0 && num_args == 2) {
            
            float val;
            sscanf(cmd_value, "%f", &val);

            print_send_data(can_id, axis_id, 0x0C, val);
            // eth.send_message(can_id, axis_id, 
            //     0x07, static_cast<uint32_t>(val));
            
           
        } else if (strcmp(cmd, "vel") == 0 && num_args == 2) {
            
            float val;
            sscanf(cmd_value, "%f", &val);

            print_send_data(can_id, axis_id, 0x0D, val);
            // eth.send_message(can_id, axis_id, 
            //     0x07, static_cast<uint32_t>(val));
            
           
        } else if (strcmp(cmd, "command") == 0 && num_args == 1) {
            char buffer[30];
            int temp_can_id;
            int temp_axis_id;
            int temp_cmd_id;
            bool data;
            printf("can_id: ");
            cin.getline(buffer, 30);
            sscanf(buffer, "%d", &temp_can_id);

            printf("axis_id: ");
            cin.getline(buffer, 30);
            sscanf(buffer, "%d", &temp_axis_id);

            printf("cmd_id: ");
            cin.getline(buffer, 30);
            sscanf(buffer, "%d", &temp_cmd_id);

            printf("int, uint, float, or none? ");
            cin.getline(buffer, 30);

            if (strcmp(cmd_value, "int") == 0) {
                // eth.send_message(temp_can_id, temp_axis_id, 
                // temp_cmd_id, static_cast<uint32_t>(val));
            } else if (strcmp(cmd_value, "uint") == 0) {
                // eth.send_message(temp_can_id, temp_axis_id, 
                // temp_cmd_id, static_cast<uint32_t>(val));
            } else if (strcmp(cmd_value, "float") == 0) {
                // eth.send_message(temp_can_id, temp_axis_id, 
                //     temp_cmd_id, static_cast<float>(val));
            } else if (strcmp(cmd_value, "none") == 0) {
                // eth.send_message(temp_can_id, temp_axis_id, 
                // temp_cmd_id);
            } else {
                error_message();
            }

            print_send_data(temp_can_id, temp_axis_id, 
                temp_cmd_id, 5);
            
        } else if (strcmp(cmd, "help") == 0 && num_args == 1) {
            help_message();
        } else {
            error_message();
        }

    }

    return 0 ;
}

void print_send_data(int can_id, int axis_id, int cmd_id) {
    printf("can_id: %x\n", can_id);
    printf("axis_id: %x\n", axis_id);
    printf("cmd_id: %x\n", cmd_id);
}

void print_send_data(int can_id, int axis_id, int cmd_id, int val) {
    printf("can_id: %x\n", can_id);
    printf("axis_id: %x\n", axis_id);
    printf("cmd_id: %x\n", cmd_id);
    printf("val: %x\n", val);
}

void print_send_data(int can_id, int axis_id, int cmd_id, float val) {
    printf("can_id: %x\n", can_id);
    printf("axis_id: %x\n", axis_id);
    printf("cmd_id: %x\n", cmd_id);
    printf("val: %f\n", val);
}

void error_message() {
    printf("Invalid command!\n");
    printf("Type 'help' for more information.\n");
}

void help_message() {
    char help_string[] = 
    "Commands:\n\
     odrive [can_id] - switches to odrive [can_id]\n\
     axis [axis_id] - switches to axis [axis_id]\n\
      for the current odrive \n\
     axis_state [options] - change the current axis state\n\
      CALIB\n\
      IDLE\n\
      CONTROL\n\
     control_mode [options] - change the current axis' control mode\n\
      POS\n\
      VEL\n\
     pos [val] - in position mode, change the position to [val]\n\
     vel [val] - in velocity mode, change the velocity to [val]\n\
     command - input a specific CAN command\n";

    printf("%s", help_string);
}