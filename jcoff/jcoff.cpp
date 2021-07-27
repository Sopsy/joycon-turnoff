// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// #define NOMINMAX
#include "windows.h"
#include "atlstr.h"
#include "jcoff.h"

#define HID_BUFFER_LENGTH 0x40
#define MAX_STR 10000

#define NINTENDO_VID (0x57e)
#define JOY_CON_L_PID (0x2006)
#define JOY_CON_R_PID (0x2007)
#define PRO_CONTROLLER_PID (0x2009)
#define SNES_CONTROLLER_PID (0x2017)
#define CHARGING_GRIP_PID (0x200e)

#define CMD_SET_INPUT_REPORT_MODE (0x03)
#define CMD_SET_HCI_STATE (0x06)
#define CMD_RESET_PAIRING_INFO (0x07)
#define CMD_SET_SHIPMENT_LOW_POWER_STATE (0x08)
#define CMD_SET_LEDS (0x30)

using namespace System::Windows;

#pragma comment(lib, "SetupAPI")

bool enable_traffic_dump = false;
bool bluetooth = true;
uint8_t global_count = 0;
hid_device* handle;
unsigned char buf[HID_BUFFER_LENGTH];

void show_error(wchar_t* title, unsigned char* buf, int length)
{
    wchar_t ret[MAX_STR];
    wchar_t data[MAX_STR];
    wchar_t temp[MAX_STR];
    for (unsigned int i = 0; i < HID_BUFFER_LENGTH; i++) {
        swprintf(temp, MAX_STR, L"%02X ", buf[i]);
        wcsncat(data, temp, MAX_STR);
    }
    swprintf(ret, MAX_STR, L"Error: %ls\n\n\nBytes read/written: %d\nData:\n%ls", title, length, data);
    MessageBox(NULL, ret, L"ERROR", MB_ICONERROR | MB_OK);

}

void joycon_send_subcommand(hid_device* handle, int subcommand, uint8_t* data, int data_length)
{
    memset(buf, 0, HID_BUFFER_LENGTH);

    buf[0] = 0x01; // Command
    buf[1] = (global_count++) & 0xF; // Packet number

    uint8_t rumble_base[8] = { 0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40 };
    memcpy(buf + 2, rumble_base, 8);

    buf[10] = subcommand;
    memcpy(buf + 11, data, data_length);

    int res = hid_write(handle, buf, HID_BUFFER_LENGTH);
    if (res == -1) {
        show_error(L"HID write error when sending a command!", buf, res);
    }

    // The Pro Controller at least does not like receiving commands too fast
    Sleep(100);

    if (subcommand == CMD_SET_HCI_STATE && data == 0x0) {
        // Don't try to read the response after commanding the controller to turn off
        return;
    }

    memset(buf, 0, HID_BUFFER_LENGTH);
    res = hid_read_timeout(handle, buf, HID_BUFFER_LENGTH, 1000);
    if (res == -1) {
        show_error(L"HID read error when sending a command!", buf, res);
    }
}

void set_leds(hid_device* handle, uint8_t status)
{
    unsigned char data[1] = { status };
    joycon_send_subcommand(handle, CMD_SET_LEDS, data, 1);
}

void set_shipment_low_power(hid_device* handle)
{
    unsigned char enable[1] = { 0x01 };
    joycon_send_subcommand(handle, CMD_SET_SHIPMENT_LOW_POWER_STATE, enable, 1);
}

void reset_pairing_information(hid_device* handle)
{
    joycon_send_subcommand(handle, CMD_RESET_PAIRING_INFO);
}

void turn_off(hid_device* handle)
{
    joycon_send_subcommand(handle, CMD_SET_HCI_STATE);
}

void set_input_report_mode(hid_device* handle)
{
    unsigned char simple_hid_mode[1] = { 0x3f };
    joycon_send_subcommand(handle, CMD_SET_INPUT_REPORT_MODE, simple_hid_mode, 1);
}

int device_connection()
{
    // Joy-Con (L)
    if (handle = hid_open(NINTENDO_VID, JOY_CON_L_PID, nullptr)) {
        device_type = 1;
        return 1;
    }

    // Joy-Con (R)
    if (handle = hid_open(NINTENDO_VID, JOY_CON_R_PID, nullptr)) {
        device_type = 2;
        return 2;
    }

    // Pro Controller
    if (handle = hid_open(NINTENDO_VID, PRO_CONTROLLER_PID, nullptr)) {
        device_type = 3;
        return 3;
    }

    // SNES Controller
    if (handle = hid_open(NINTENDO_VID, SNES_CONTROLLER_PID, nullptr)) {
        device_type = 4;
        return 4;
    }

    // Nothing found
    return 0;
}

wchar_t* get_controller_name(int device_type)
{
    switch (device_type)
    {
    case 1:
        return L"Joy-Con (L)";
    case 2:
        return L"Joy-Con (R)";
    case 3:
        return L"Pro Controller";
    case 4:
        return L"SNES Controller";
    default:
        return L"Unknown controller, maybe it is not wise to continue!";
    }
}

wchar_t* get_device_info()
{
    int res;
    wchar_t mfg[MAX_STR];
    wchar_t prod[MAX_STR];
    wchar_t sn[MAX_STR];
    wchar_t ret[MAX_STR];

    res = hid_get_manufacturer_string(handle, mfg, MAX_STR);
    res = hid_get_product_string(handle, prod, MAX_STR);
    res = hid_get_serial_number_string(handle, sn, MAX_STR);

    swprintf(ret, MAX_STR, L"%ls %ls\nType: %ls\nHID serial number: %ls", mfg, prod, get_controller_name(device_type), sn);

    return ret;
}

int Main()
{
    const wchar_t title[100] = L"Shutdown Joy-Con";

    int warning_result = MessageBox(NULL,
        L"WARNING: This program will write to the SPI flash of your\n"
        L"connected Nintendo Switch controller. This can brick your controller.\n\n"
        L"CONTINUE AT YOUR OWN RISK!\n\n"
        L"IMPORTANT: The controller needs to be connected via Bluetooth or this will not work!\n\n"
        L"If all goes well, your controller will be left in a state like it was sent from the factory:\n"
        L"- Low battery consumption (will not die in a week)\n- Pressing buttons will not turn it on\n- Pairing information is removed\n\n"
        L"To use the controller again, you will need to reattach it to your Switch. "
        L"Bluetooth pairing can also be initiated by long-pressing the Sync-button."
        , title, MB_ICONWARNING | MB_OKCANCEL);

    if (warning_result == IDCANCEL) {
        return 1;
    }

    while (!device_connection()) {
        int connection_retry_result = MessageBox(NULL,
            L"No Joy-Con was detected!\n\n"
            L"Please connect the controller with Bluetooth and click retry.\n"
            L"Long press the sync button on the controller to turn on pairing mode.\n",
            title, MB_RETRYCANCEL | MB_ICONERROR);
        
        if (connection_retry_result == IDCANCEL) {
            return 1;
        }
    }

    set_input_report_mode(handle);
    set_leds(handle, 0b11110000);

    wchar_t message[MAX_STR] = L"Controller found!\n\n";
    wcsncat(message, get_device_info(), MAX_STR);
    wcsncat(message,
        L"\n\nPlease confirm that all the LEDs on the controller you want to turn off are flashing at the same time. "
        L"If they are not, please cancel now and verify you are connecting the controller with Bluetooth!"
        L"\n\nPress OK to turn the controller off.", MAX_STR);

    int info_result = MessageBox(NULL,
        message, title,
        MB_ICONINFORMATION | MB_OKCANCEL);

    if (info_result != IDOK) {
        set_leds(handle, 0b00000001);
        return 1;
    }

    set_shipment_low_power(handle);
    reset_pairing_information(handle);
    turn_off(handle);

    MessageBox(NULL, L"Controller has been turned off.", title, MB_OK);

    return 0;
}
