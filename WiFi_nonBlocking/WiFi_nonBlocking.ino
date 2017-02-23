/*by Me No Dev
@me-no-dev
Fev 02 10:11
*/
#include "WiFi.h"

#define STA_SSID "your-ssid"
#define STA_PASS "your-password"
#define AP_SSID  "esp32"

static volatile bool wifi_connected = false;

void WiFiEvent(WiFiEvent_t event){
    switch(event) {

        case SYSTEM_EVENT_AP_START:
            //can set ap hostname here
            WiFi.softAPsetHostname(AP_SSID);
            //enable ap ipv6 here
            WiFi.softAPenableIpV6();
            break;

        case SYSTEM_EVENT_STA_START:
            //set sta hostname here
            WiFi.setHostname(AP_SSID);
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            //enable sta ipv6 here
            WiFi.enableIpV6();
            break;
        case SYSTEM_EVENT_AP_STA_GOT_IP6:
            //both interfaces get the same event
            Serial.print("STA IPv6: ");
            Serial.println(WiFi.localIPv6());
            Serial.print("AP IPv6: ");
            Serial.println(WiFi.softAPIPv6());
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            wifiOnConnect();
            wifi_connected = true;
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            wifi_connected = false;
            wifiOnDisconnect();
            break;
        default:
            break;
    }
}

void setup(){
    Serial.begin(115200);
    WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAP(AP_SSID);
    WiFi.begin(STA_SSID, STA_PASS);
    Serial.println("AP Started");
    Serial.print("AP SSID: ");
    Serial.println(AP_SSID);
    Serial.print("AP IPv4: ");
    Serial.println(WiFi.softAPIP());
}

void loop(){
    if(wifi_connected){
        wifiConnectedLoop();
    } else {
        wifiDisconnectedLoop();
    }
}

//when wifi connects
void wifiOnConnect(){
    Serial.println("STA Connected");
    Serial.print("STA SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("STA IPv4: ");
    Serial.println(WiFi.localIP());
    Serial.print("STA IPv6: ");
    Serial.println(WiFi.localIPv6());
}

//when wifi disconnects
void wifiOnDisconnect(){
    Serial.println("STA Disconnected");
    delay(1000);
    WiFi.begin(STA_SSID, STA_PASS);
}

//while wifi is connected
void wifiConnectedLoop(){
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
    delay(1000);
}

//while wifi is not connected
void wifiDisconnectedLoop(){

}
