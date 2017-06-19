/*Simple example of multi loop
* By Evandro Luis Copercini
* Based on pcbreflux video
* Public domain license 2017
*/

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void loop1(void *pvParameters) {
  while (1) {
     Serial.println("loop1");
     delay(1000);
  }
}

void loop2(void *pvParameters) {
  while (1) {
     Serial.println("loop2");
     delay(300);
  }
}

void loop3(void *pvParameters) {
  while (1) {
     Serial.println("loop3");
     delay(4000);
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(loop3, "loop3", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
   Serial.println("loop0");
   delay(5000);
}
