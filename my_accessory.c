/*
 * my_accessory.c
 * Define the accessory in C language using the Macro in characteristics.h
 *
 *  Created on: 2020-05-15
 *      Author: Mixiaoxiao (Wang Bin)
 */

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

// Called to identify this accessory. See HAP section 6.7.6 Identify Routine
// Generally this is called when paired successfully or click the "Identify Accessory" button in Home APP.
void my_accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
}

// See HAP section 8.41 and characteristics.h

//插座开关
homekit_characteristic_t cha_outlet_switch = HOMEKIT_CHARACTERISTIC_(ON, false);
//是否在使用
homekit_characteristic_t cha_outlet_in_use = HOMEKIT_CHARACTERISTIC_(OUTLET_IN_USE, false);
//名称
homekit_characteristic_t cha_outlet_name = HOMEKIT_CHARACTERISTIC_(NAME, "PC-LRY");

//重启开关
homekit_characteristic_t cha_reboot_switch = HOMEKIT_CHARACTERISTIC_(ON, false);
//重启开关名称
homekit_characteristic_t cha_reboot_name = HOMEKIT_CHARACTERISTIC_(NAME, "Reboot");

//强制关机开关
homekit_characteristic_t cha_shutdown_switch = HOMEKIT_CHARACTERISTIC_(ON, false);
//重启开关名称
homekit_characteristic_t cha_shutdown_name = HOMEKIT_CHARACTERISTIC_(NAME, "Forced Shutdown");



homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_outlet, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Temperature Sensor"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arduino HomeKit"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266/ESP32"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),
        HOMEKIT_SERVICE(OUTLET, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
            &cha_outlet_switch,
			      &cha_outlet_in_use,
            &cha_outlet_name,
            NULL
        }),
        NULL
    }),
    HOMEKIT_ACCESSORY(.id=2, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Switch"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            &cha_reboot_switch,
            &cha_reboot_name,
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .characteristics=(homekit_characteristic_t*[]){
            &cha_shutdown_switch,
            &cha_shutdown_name,
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "222-22-222"
};
