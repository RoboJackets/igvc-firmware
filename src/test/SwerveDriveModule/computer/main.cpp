#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include "communication/ParseProtobuf.h"
// #include "pthread.h"

#define MAX_ODRIVE 1
#define MAX_AXIS 1

using namespace std;

struct drive_data {
    bool isDriveControl;
    char keyDown;
};


void help_message();
void error_message();
void print_send_data(int can_id, int axis_id, int cmd_id);
void print_send_data(int can_id, int axis_id, int cmd_id, int val);
void print_send_data(int can_id, int axis_id, int cmd_id, float val);
void handle_drive_control(ParseProtobuf *eth, drive_data *d_data);
int keypress();
void send_drive_data(ParseProtobuf *eth, drive_data *d_data);

mutex drive_data_mutex;

mutex recv_message_mutex;
mutex send_message_ready_mutex;
mutex send_message_mutex;

void handle_input(ParseProtobuf *eth, drive_data *d_data);
void handle_ethernet(ParseProtobuf *eth, drive_data *d_data);

int can_id_arr[4][2] = {
    0x3, 0x5, 
    0x13, 0x15,
    0x23, 0x25,
    0x33, 0x35
};

int main() {

    ParseProtobuf eth = ParseProtobuf(); 
    eth.connect();

    drive_data d_data;
    d_data.isDriveControl = false;

    // thread_args *args = new thread_args;
    // args->eth = &eth;
    // *(args->isDriveControl) = false;
    // *(args->keyDown) = -1;

    help_message();
    
    // pthread_mutex_init(&send_message_mutex, NULL);
    // pthread_mutex_init(&send_message_ready_mutex, NULL);
    // pthread_mutex_init(&recv_message_mutex, NULL);

    thread ethernet_thread(handle_ethernet, &eth, &d_data);
    thread input_thread(handle_input, &eth, &d_data);

    // pthread_create(&ethernet_thread, NULL, handle_ethernet, (void *)args);
    // pthread_create(&input_thread, NULL, handle_input, (void*)args);

    while (true) {
        
    }

    return 0 ;
}

void handle_ethernet(ParseProtobuf *eth, drive_data *d_data) {

    while (true) {

        // pthread_mutex_lock(&recv_message_mutex);
        // eth.recieve_message();
        // pthread_mutex_unlock(&recv_message_mutex);

        // pthread_mutex_lock(&send_message_mutex);
        // pthread_mutex_lock(&send_message_ready_mutex);

        send_message_mutex.lock();
        send_message_ready_mutex.lock();

        // if (eth.get_request_message_ready() == true) {
        if (eth->get_request_message_ready() == true) {
            // printf("Success!\n");
            // message ready, send it!
            eth->send_message();
            // reset the flag
            eth->set_request_message_ready(false);
            // send_message_ready = false;
        } 
        else {
            // message not ready, send ACK
            eth->populate_ack();
            eth->send_message();
        }

        // pthread_mutex_unlock(&send_message_ready_mutex);
        // pthread_mutex_unlock(&send_message_mutex);

        send_message_ready_mutex.unlock();
        send_message_mutex.unlock();

        std::this_thread::sleep_for(50ms);
    }
}

void handle_input(ParseProtobuf *eth, drive_data *d_data) {
    char input[30];
    char delim[] = " ";
    char *ptr;

    int num_args;
    char cmd[15];
    char cmd_value[32];

    int odrive = 0;
    int axis_id = 0; 

    // bool *isDriveControl = args->isDriveControl;
    // int *keyDown = args->keyDown;

    while (true) {

        drive_data_mutex.lock();
        if (d_data->isDriveControl) {
            drive_data_mutex.unlock();

            handle_drive_control(eth, d_data);

            drive_data_mutex.lock();
            d_data->isDriveControl = false;
            drive_data_mutex.unlock();

            continue;
        }
        drive_data_mutex.unlock();

        printf(" > ");
        cin.getline(input, 30);

        ptr = strtok(input, delim);
        memcpy(cmd, ptr, 15);
        num_args = 1;
        // printf("%s\n", cmd);
        ptr = strtok(nullptr, delim);
        if (ptr != nullptr) {
            memcpy(cmd_value, ptr, 32);
            num_args = 2;
            printf("%s\n", cmd_value);
        }
            
        if (strcmp(cmd, "odrive") == 0 && num_args == 2) {
            int val; 
            sscanf(cmd_value, "%d", &val);
            
            if (val >= 0 && val <= MAX_ODRIVE) {
                odrive = val;
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
                    axis_id, odrive);
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
                print_send_data(can_id_arr[odrive][axis_id], axis_id, 0x07, val);

                // pthread_mutex_lock(&send_message_mutex);
                // pthread_mutex_lock(&send_message_ready_mutex);
                send_message_mutex.lock();
                send_message_ready_mutex.lock();

                eth->populate_my_message(can_id_arr[odrive][axis_id], axis_id, 
                    0x07, static_cast<uint32_t>(val));
                // pthread_mutex_unlock(&send_message_ready_mutex);
                // pthread_mutex_unlock(&send_message_mutex);

                send_message_ready_mutex.unlock();
                send_message_mutex.unlock();
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
                print_send_data(can_id_arr[odrive][axis_id], axis_id, 0x0B, val);

                // pthread_mutex_lock(&send_message_mutex);
                send_message_mutex.lock();
                eth->populate_my_message(can_id_arr[odrive][axis_id], axis_id, 
                    0x0B, static_cast<uint32_t>(val));
                // pthread_mutex_unlock(&send_message_mutex);
                send_message_mutex.unlock();
            }
           
        } else if (strcmp(cmd, "pos") == 0 && num_args == 2) {
            
            float val;
            sscanf(cmd_value, "%f", &val);

            print_send_data(can_id_arr[odrive][axis_id], axis_id, 0x0C, val);

            // pthread_mutex_lock(&send_message_mutex);
            // pthread_mutex_lock(&send_message_ready_mutex);
            send_message_mutex.lock();
            send_message_ready_mutex.lock();
            
            eth->populate_my_message(can_id_arr[odrive][axis_id], axis_id, 
                0x0C, static_cast<uint32_t>(val));
            // pthread_mutex_unlock(&send_message_ready_mutex);
            // pthread_mutex_unlock(&send_message_mutex);
            send_message_ready_mutex.unlock();
            send_message_mutex.unlock();
           
        } else if (strcmp(cmd, "vel") == 0 && num_args == 2) {
            
            // float val;
            // sscanf(cmd_value, "%f", &val);

            float val;
            sscanf(cmd_value, "%f", &val);

            printf("0x%x\n", *(unsigned int*)&val);

            print_send_data(can_id_arr[odrive][axis_id], axis_id, 0x0D, static_cast<float>(val));

            // pthread_mutex_lock(&send_message_mutex);
            // pthread_mutex_lock(&send_message_ready_mutex);
            send_message_mutex.lock();
            send_message_ready_mutex.lock();
            
            eth->populate_message(can_id_arr[odrive][axis_id], axis_id, 
                0x0D, static_cast<float>(val));
            // pthread_mutex_unlock(&send_message_ready_mutex);
            // pthread_mutex_unlock(&send_message_mutex);
            send_message_ready_mutex.unlock();
            send_message_mutex.unlock();
           
        } else if(strcmp(cmd, "drive") == 0 && num_args == 1) {

            // // axis_state CONTROL
            // int val = 8;

            // // odrive
            // for (int i = 0; i < 2; i ++) {
            //     //axis 
            //     for (int j = 0; j < 2; j++) {
            //         send_message_mutex.lock();
            //         send_message_ready_mutex.lock();
            //         eth->populate_my_message(can_id_arr[i][j], j, 
            //             0x07, static_cast<uint32_t>(val));
            //         send_message_ready_mutex.unlock();
            //         send_message_mutex.unlock();
                    

            //         std::this_thread::sleep_for(100ms);
            //     }
            // }

            drive_data_mutex.lock();
            d_data->isDriveControl = true;
            drive_data_mutex.unlock();
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

            // pthread_mutex_lock(&send_message_mutex);
            // pthread_mutex_lock(&send_message_ready_mutex);
            send_message_mutex.lock();
            send_message_ready_mutex.lock();
            
            if (strcmp(cmd_value, "int") == 0) {
                eth->populate_my_message(temp_can_id, temp_axis_id, 
                    temp_cmd_id, static_cast<uint32_t>(data));
            } else if (strcmp(cmd_value, "uint") == 0) {
                eth->populate_my_message(temp_can_id, temp_axis_id, 
                    temp_cmd_id, static_cast<uint32_t>(data));
            } else if (strcmp(cmd_value, "float") == 0) {
                eth->populate_my_message(temp_can_id, temp_axis_id, 
                    temp_cmd_id, static_cast<float>(data));
            } else if (strcmp(cmd_value, "none") == 0) {
                eth->populate_message(temp_can_id, temp_axis_id, 
                    temp_cmd_id);
            } else {
                error_message();
            }
            // pthread_mutex_unlock(&send_message_ready_mutex);
            // pthread_mutex_unlock(&send_message_mutex);
            send_message_ready_mutex.unlock();
            send_message_mutex.unlock();

            print_send_data(temp_can_id, temp_axis_id, 
                temp_cmd_id, 5);
            
        } else if (strcmp(cmd, "help") == 0 && num_args == 1) {
            help_message();
        } else {
            error_message();
        }

    }
    
}

void handle_drive_control(ParseProtobuf *eth, drive_data *d_data) {

    printf("You are controlling the test rig using WASD.\n");
    printf("You can only control a single odrive at a time\n");

    thread send_data_thread(send_drive_data, eth, d_data);

    while (true){
        // PROBLEM: this is a blocking call!
        int key = keypress();
        drive_data_mutex.lock();
            d_data->keyDown = (char)key;
            d_data->isDriveControl = true;
        drive_data_mutex.unlock();

        std::cout << key << "\n";
    }
    
    return;
}

void send_drive_data(ParseProtobuf *eth, drive_data *d_data) {

    float velocity;
    int odrive = 0;
    int axis_id = 0;

    bool button_down = false;
    bool prev_button_down = false;
    char prev_char = 0;

    while (true) {

        drive_data_mutex.lock();
        if (d_data->isDriveControl) {
                    
            d_data->isDriveControl = false;

            if (d_data->keyDown == 'a') {
                axis_id = 0;
                velocity = 1;
                button_down = true;
                prev_char = 'a';
            } else if (d_data->keyDown == 'd') {
                axis_id = 0;
                velocity = -1;
                button_down = true;
                prev_char = 'd';
            } else if (d_data->keyDown == 'w') {
                axis_id = 1;
                velocity = 1;
                button_down = true;
                prev_char = 'w';
            } else if (d_data->keyDown == 's') {
                axis_id = 1;
                velocity = -1;
                button_down = true;
                prev_char = 's';
            } else if (d_data->keyDown == '0') {
                velocity = 0;
                odrive = 0;
            } else if (d_data->keyDown == '1') {
                velocity = 0;
                odrive = 1;
            } else {
                velocity = 0;
                button_down = false;
            }

            drive_data_mutex.unlock();

            send_message_mutex.lock();
            send_message_ready_mutex.lock();
            eth->populate_message(can_id_arr[odrive][axis_id], axis_id, 
                0x0D, velocity);
            send_message_ready_mutex.unlock();
            send_message_mutex.unlock();
            
        } else {
            button_down = false;
            d_data->isDriveControl = false;
            drive_data_mutex.unlock();

            if (prev_button_down) {
                send_message_mutex.lock();
                send_message_ready_mutex.lock();
                eth->populate_message(can_id_arr[odrive][axis_id], axis_id, 
                    0x0D, 0);
                send_message_ready_mutex.unlock();
                send_message_mutex.unlock();
            }
        }

        prev_button_down = button_down;

        this_thread::sleep_for(200ms);
    }
    
}

int keypress() {
    system ("/bin/stty raw");
    int c;
    system ("/bin/stty -echo");
    c = getc(stdin);
    system ("/bin/stty echo");
    system ("/bin/stty cooked");
    return c;
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