#pragma once
#include <cstdint>
#include "hidapi.h"

extern void show_error(wchar_t* title, unsigned char* buf, int length);
extern void joycon_send_subcommand(hid_device* handle, int subcommand, uint8_t* data = 0x0, int data_length = 0);
extern void set_leds(hid_device* handle, uint8_t status);
extern void set_shipment_low_power(hid_device* handle);
extern void reset_pairing_information(hid_device* handle);
extern void turn_off(hid_device* handle);
extern void set_input_report_mode(hid_device* handle);
extern int device_connection();
extern wchar_t* get_controller_name(int device_type);
extern wchar_t* get_device_info();

int device_type;