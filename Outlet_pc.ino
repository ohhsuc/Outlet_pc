#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
	Serial.begin(115200);
	wifi_connect(); // in wifi_info.h
	my_homekit_setup();
}

void loop() {
	my_homekit_loop();
	delay(10);
}

//==============================
// Homekit setup and loop
//==============================

// access your homekit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_outlet_switch; // 插座开关
extern "C" homekit_characteristic_t cha_reboot_switch; // 重启开关
extern "C" homekit_characteristic_t cha_shutdown_switch; // 强制关机

static uint32_t next_heap_millis = 0;
static uint32_t next_report_millis = 0;

//开关机控制
#define ONOFF 4
//重启控制
#define REBOOT 5
//电脑状态检测
#define STATE 13
//存储状态
int preState = -1;

//从APP中下发开关机
void cha_outlet_switch_setter(const homekit_value_t value) {
  bool onoff_temp = value.bool_value;
  cha_outlet_switch.value.bool_value = onoff_temp;  //sync the value
  LOG_D("OnOff Switch: %s", onoff_temp ? "ON" : "OFF");
  
  //电脑为开机，下发关机时
  if(digitalRead(STATE) == HIGH && !onoff_temp){
    //执行关机
    digitalWrite(ONOFF, HIGH);
    delay(1000);
    digitalWrite(ONOFF, LOW);
  }

  //电脑为关机，下发开机时
  if(digitalRead(STATE) == LOW && onoff_temp){
    //执行开机
    digitalWrite(ONOFF, HIGH);
    delay(1000);
    digitalWrite(ONOFF, LOW);
  }

}

//从APP中下发重启
void cha_reboot_switch_setter(const homekit_value_t value) {
  bool reboot_temp = value.bool_value;
  cha_reboot_switch.value.bool_value = reboot_temp;  //sync the value
  LOG_D("Reboot Switch: %s", reboot_temp ? "ON" : "OFF");
  
  //电脑为开机状态
  if(digitalRead(STATE) == HIGH && reboot_temp){
    digitalWrite(REBOOT,HIGH);
    delay(1000);
    digitalWrite(REBOOT,LOW);
    //关闭按钮
    cha_reboot_switch.value.bool_value = false;
    homekit_characteristic_notify(&cha_reboot_switch, cha_reboot_switch.value);
  }
}

//从APP中下发强制关机
void cha_shutdown_switch_setter(const homekit_value_t value) {
  bool shutdown_temp = value.bool_value;
  cha_shutdown_switch.value.bool_value = shutdown_temp;  //sync the value
  LOG_D("Shutdown Switch: %s", shutdown_temp ? "ON" : "OFF");

  //电脑为开机状态
  if(digitalRead(STATE) == HIGH && shutdown_temp){
    digitalWrite(ONOFF,HIGH);
    delay(3500); // 长按三秒半
    digitalWrite(ONOFF,LOW);
    //关闭按钮
    cha_shutdown_switch.value.bool_value = false;
    homekit_characteristic_notify(&cha_shutdown_switch, cha_shutdown_switch.value);
  }
}

void my_homekit_setup() {
  //设置引脚模式并初始化引脚
  pinMode(ONOFF, OUTPUT); //开关机控制引脚
  digitalWrite(ONOFF, LOW);
  pinMode(REBOOT, OUTPUT); //重启控制引脚
  digitalWrite(REBOOT, LOW);
  pinMode(STATE, INPUT); //电脑状态检测引脚
  //绑定开关机下发函数
  cha_outlet_switch.setter = cha_outlet_switch_setter;
  //绑定重启开关
  cha_reboot_switch.setter = cha_reboot_switch_setter;
  //绑定强制关机
  cha_shutdown_switch.setter = cha_shutdown_switch_setter;
	arduino_homekit_setup(&config);
}

void my_homekit_loop() {
	arduino_homekit_loop();
	const uint32_t t = millis();
  if (t > next_report_millis) {
    next_report_millis = t + 3 * 1000;
    if(preState != digitalRead(STATE)){
      pc_state_report(); // 每3秒监测电脑状态调用上报
      preState = digitalRead(STATE);
    }
  }
	if (t > next_heap_millis) {
		// show heap info every 5 seconds
		next_heap_millis = t + 5 * 1000;
		LOG_D("Free heap: %d, HomeKit clients: %d",
				ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
	}
}

//电脑状态上报
void pc_state_report() {
  //LOW为开机 HIGHT为关机
  if(digitalRead(STATE) == HIGH){
    //上报当前状态
    cha_outlet_switch.value.bool_value = true;
    homekit_characteristic_notify(&cha_outlet_switch, cha_outlet_switch.value);
    LOG_D("State Report: Startup");
  }else{
    cha_outlet_switch.value.bool_value = false;
    homekit_characteristic_notify(&cha_outlet_switch, cha_outlet_switch.value);
    LOG_D("State Report: Shutdown");
  }
}
