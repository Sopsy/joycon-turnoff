// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// #define NOMINMAX
#include "windows.h"
#include "atlstr.h"
#include "jcoff.h"
#include "hidapi.h"

using namespace System::Windows;

#pragma comment(lib, "SetupAPI")

bool enable_traffic_dump = false;

hid_device *handle;

int set_leds(u8 status) {
    int res;
    u8 buf[49];
    memset(buf, 0, sizeof(buf));
    auto hdr = (brcm_hdr *)buf;
    auto pkt = (brcm_cmd_01 *)(hdr + 1);
    hdr->cmd = 0x01;
    hdr->timer = timing_byte & 0xF;
    timing_byte++;
    pkt->subcmd = 0x30;
    pkt->subcmd_arg.arg1 = status;
    res = hid_write(handle, buf, sizeof(buf));
    res = hid_read_timeout(handle, buf, 0, 64);

    return 0;
}

int set_shipment_low_power() {
    int res;
    u8 buf[49];
    memset(buf, 0, sizeof(buf));
    auto hdr = (brcm_hdr *)buf;
    auto pkt = (brcm_cmd_01 *)(hdr + 1);
    hdr->cmd = 0x01;
    hdr->timer = timing_byte & 0xF;
    timing_byte++;
    pkt->subcmd = 0x08;
    pkt->subcmd_arg.arg1 = 0x01;
    res = hid_write(handle, buf, sizeof(buf));
    res = hid_read_timeout(handle, buf, 0, 64);

    return 0;
}

int reset_pairing_information() {
    int res;
    u8 buf[49];
    memset(buf, 0, sizeof(buf));
    auto hdr = (brcm_hdr *)buf;
    auto pkt = (brcm_cmd_01 *)(hdr + 1);
    hdr->cmd = 0x01;
    hdr->timer = timing_byte & 0xF;
    timing_byte++;
    pkt->subcmd = 0x07;
    pkt->subcmd_arg.arg1 = 0x00;
    res = hid_write(handle, buf, sizeof(buf));
    res = hid_read_timeout(handle, buf, 0, 64);

    return 0;
}

int turn_off() {
    int res;
    u8 buf[49];
    memset(buf, 0, sizeof(buf));
    auto hdr = (brcm_hdr *)buf;
    auto pkt = (brcm_cmd_01 *)(hdr + 1);
    hdr->cmd = 0x01;
    hdr->timer = timing_byte & 0xF;
    timing_byte++;
    pkt->subcmd = 0x06;
    pkt->subcmd_arg.arg1 = 0x00;
    res = hid_write(handle, buf, sizeof(buf));
    res = hid_read_timeout(handle, buf, 0, 64);

    return 0;
}

int device_connection() {
    // Joy-Con (L)
    if (handle = hid_open(0x57e, 0x2006, nullptr)) {
        handle_ok = 1;
        return 1;
    }

    // Joy-Con (R)
    if (handle = hid_open(0x57e, 0x2007, nullptr)) {
        handle_ok = 2;
        return 2;
    }

    // Pro Controller
    if (handle = hid_open(0x57e, 0x2009, nullptr)) {
        handle_ok = 3;
        return 3;
    }

    // SNES Controller
    if (handle = hid_open(0x57e, 0x2017, nullptr)) {
        handle_ok = 4;
        return 4;
    }

    // Nothing found
    return 0;
}

static wchar_t* charToWChar(const char* text)
{
    const size_t size = strlen(text) + 1;
    wchar_t* wText = new wchar_t[size];
    mbstowcs(wText, text, size);
    return wText;
}

int Main(array<String^>^ args) {
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

    timing_byte = 0x0;

    set_leds(0xf0);
        
    wchar_t* controller_name;
    if (handle_ok == 1) {
        controller_name = L"Joy-Con (L)";
    } else if (handle_ok == 2) {
        controller_name = L"Joy-Con (R)";
    } else if (handle_ok == 3) {
        controller_name = L"Pro Controller";
    } else if (handle_ok == 4) {
        controller_name = L"SNES Controller";
    } else {
        controller_name = L"Unknown controller, maybe it is not wise to continue!";
    }

    wchar_t message[1000] = L"Detected controller: ";
    wcsncat(message, controller_name, 1000);
    wcsncat(message,
        L"\n\nPlease confirm that all the LEDs on the controller you want to turn off are flashing at the same time. "
        L"If they are not, please cancel now and verify you are connecting the controller with Bluetooth!"
        L"\n\nPress OK to turn the controller off.", 1000);

    int info_result = MessageBox(NULL,
        message, title,
        MB_ICONINFORMATION | MB_OKCANCEL);

    if (info_result != IDOK) {
        set_leds(0x1);
        return 1;
    }

    set_shipment_low_power();
    reset_pairing_information();
    turn_off();

    MessageBox(NULL, L"Controller has been turned off.", title, MB_OK);

    return 0;
}
