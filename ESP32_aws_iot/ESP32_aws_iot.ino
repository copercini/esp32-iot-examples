/* This example uses an ESP32 to connect to AWS IoT using X.509 certificates.
by Evandro Luis Copercini - Public domain - 2017

1- Create a thing http://docs.aws.amazon.com/iot/latest/developerguide/register-device.html
and generate a device certificate http://docs.aws.amazon.com/iot/latest/developerguide/create-device-certificate.html

2- Edit this code with your Wifi credentials, AWS IoT endpoint and certificates.

3- RUN! 
*/

#include <WiFiClientSecure.h>
#include <MQTTClient.h>   //you need to install this library: https://github.com/256dpi/arduino-mqtt

const char *ssid = "your_wifi_name";
const char *pass = "your_wifi_password";

const char *awsEndPoint = "xxxxxxxxxxxxx.iot.yy-yyyy-z.amazonaws.com";
const char *subscribeTopic = "inTopic"; // Can be changed to shadow topics...
const char *publishTopic = "outTopic";

WiFiClientSecure net;
MQTTClient client;

unsigned long lastMillis = 0;

long lastMsg = 0;
char msg[50];
int value = 0;


/* root CA can be downloaded in:
	https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
*/
char* rootCABuff = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n" \
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n" \
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n" \
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n" \
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n" \
"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n" \
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n" \
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n" \
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n" \
"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n" \
"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n" \
"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n" \
"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n" \
"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n" \
"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n" \
"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n" \
"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n" \
"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n" \
"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n" \
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n" \
"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n" \
"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n" \
"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n" \
"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n" \
"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n" \
"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\n" \
"-----END CERTIFICATE-----\n";

// Fill your certificate.pem.crt with LINE ENDING
char* certificateBuff = \
"-----BEGIN CERTIFICATE-----\n" \
"144gW4ppxkKgxw4Wxg4Vx4a4nOX/Wz2zxt3ZSa/NWPg4W4qw1x0GpSnGS4W3gnzW\n" \
"pwgx1z0xSzW4WgNVWxs1nkWtqXpvW4WXZW4gg2Vqg124ZX1gTz1WWWW6W24gq29t\n" \
"4z2gqq4gTg1TZWW0gGx24WNgPVghp2hpW1g0W24gnz1VgzxzWw0xNzxx14gxN444\n" \
"NgNxWw00OTzq1zzq1zgtNT2x1W4xHgxxWgNVWx11z0WXgqW4W1ngn2VqgG21xWNh\n" \
"gGgwggz41x0GpSnGS4W3gnzWxngxx44WgwxwggzKxo4WxnptqW4g6o9t2atWzTq9\n" \
"np3gg4W2K22TqptgW41kn6vtqGNOG/xH0g91/2Np44NZ3zx0O4x4tgGzH4qH20OK\n" \
"G4V2TtOgs4W3/19pqxVxNWxgxtx44p111ggag1On/kgZZ44x34xvgWg0xw4zq2pz\n" \
"n4npWpWnwaW2xOsZKp/6g/xpW1H9hX1SW1gq4tp9zqx2Wx4V39w0zzqt7hpW0xpp\n" \
"2n/q0vos2o4Oqgn62g4Vs81n1WWtx04ggzt4gpxzgg4Wg/VnV2PnKpg17tW4gqWn\n" \
"q1qXaZxznnWKZqgx44g4GqWz6a442Ka4pgK4ZzH4zxXVvWnpxSq6gk1Xg4Z1px1n\n" \
"NWthxg1WxxG4qgWz1W8Gx1gg4wnq1WxxWN4Wkspt4h3zko1W8Ttp2k4t9tW/1W0G\n" \
"x1ggggnWWWS7knn6WngxW1OgtgpX8WgNWWPwxgx1WgNVHg1WxW8zx4xx1x4Gx1gg\n" \
"gwzW/wnzxw4HggxNWgknhk4G9w0WxnsWxxOpxnzxWZS440gt2xpW4PXz43nXZz13\n" \
"S3WxgWoqgW1OnVaN1n4qzVzW2x29p0z2ZPw4tgxhg/t2zGtaV24WWgggWX6n4gxx\n" \
"XZVnh2ZVng12tN4X2ht/1a1psv6k34g2gs71114pt0gpV3TkSkp/3wvW2Xwt/78H\n" \
"GxW4x4Tvt8pzx4GogKgGzW2PgKS3p10z4n44pN4sNpqtpppHxPxqp3zKW7Kwxpv4\n" \
"qgW2pgW74ooHt6xWPzZ87xxq7s2xx1nS91zO9Ww21z2gpzazNtsKtxgn3Tgx2Vx8\n" \
"WgsNTz/TTvwWGwg2t2vpn4Hqo4oXnWT4p4gntatzO06z9nZt12gxxgqgWWPHwn==\n" \
"-----END CERTIFICATE-----\n";


// Fill your private.pem.key with LINE ENDING
char* privateKeyBuff = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"144zow4WxxKpxnzxgWG44OnPzZWggp08vZwt6wqxZS4tg2xgx24tp4agaWW4Thvw\n" \
"W9zWTP94xS44Wgx1gggG4WxxnhqWWtgg4h4Wgk7Tn74hg/9Wxp12pTW8qGgWgpx4\n" \
"NTxw/2ggnvtzWWq4Wtax21G0g1ppXstgnkpxg2gn6sP1q24gGSnWaoP8nnth/qV9\n" \
"gnzWG47xWX11ZgpSWg/pN13sgz4XWtGnH4gP8t262NnpT1W4atoOWWPNx4Wnzpgp\n" \
"4WHgq7nn10VpWn/1gWSz64nWNzaW47s101tp2/1pp0zxS1WHpzpxaW1WXgvoqSqv\n" \
"4XHS4GXh48wW1W1p6WksggtgW142gxn4kgVgqn4gxnxWxo4WxzXSahnSPxaq8nWp\n" \
"qxHVSOTWWxw142g9pV8Wga92q1Oavp9hPgxWan0O1OWpHqV8n8n3gvo4akT7gpw4\n" \
"Wz16pW0xt6gqxnPHp4xt6gPK2T9X43v2WWhp2nHggn6VgW9Tz34Xg48671nsHggx\n" \
"Xng89gqgghnkpg47axoxgWzp4Hka1s7ZzSnqnHgW4axtWgzhnzNvnpnn10Wq2xVz\n" \
"vn27g3t167Sgtq41WW4vGngK4vTptpN4240Zxznx4V4GX910vWW9Oxg1n0xnng34\n" \
"4gxWxNxgzWn2x4W4Xgq62NPPGxxz4p64gWWwWTxkwnzWSpztOxKo9sKT8n4a44W4\n" \
"4g1WP4gpgqzx9gpX4xVn1NggWS7nV3P19WXxzpNPx02q26WpnaV4g8W9ZZTH/g2H\n" \
"3gqox1oWgk2gWWXnx3sg2tzKn4HX4ZSzKW7gzz2zWpt9WtGgWG23q64hg124xnvz\n" \
"PX984gWqW4ztg4z2h4x4S44g8x4gpq/ZtGtg42zvZ441Gn/K9z3WHp8pgqzxt4w2\n" \
"W8ognKgtnX1txtxWnhpzGTppqgX1SggvosgVz9O11ggP1KH4xHpZ3NgspzwWVxaO\n" \
"2z3NK1sWKG/18gg1z88gWHN98O2gp42ZxqVwOznnhtT42Zp824PhhW4n1kz1hgvP\n" \
"xs1gWz1gPnzTtX99s01nvVNn1WxK1oXxwv1Pn18pgqW4V1vx1hGzg2V77gtgk4Wa\n" \
"nzaxgTNp/gpKztxg44gnVGpnWKntKhW449gn14xwnS1HZwzNSzGg14xg2ghWPHGg\n" \
"WnWg4gW7gnWz/X4pg2pPWzH4kngNzVWgp4gWgVxnZv6H2XZ12VvZSvxxn64SqW4h\n" \
"g3twngt1Vp1Wxg14gw7VgwKWgzn111WxtGn1aWO68VK1GzG1n/o8xzn14g40Pxg1\n" \
"WnW1/pnq7Ot0gXgg8xGO7Wgt24gnn6kXznggNg2xxHngg74Z4W/7W444V0Wkh2nV\n" \
"zo4Tq4gVxKoWzxpZ2zpz2gTs62x24PT4xgnggVvnq8gzp204z27On1K1P4hgzwHW\n" \
"7tg/xoGWxNxzw/pnao4zZqZkg91Gt/qqw0kgK9nW423ggxz1zP2vxnXS6hn2vv2n\n" \
"zzpg444t2pW1thtKKg8W0gqn1w1gSWgnWZKz62Wh3tWWg4W1gwO46o9WgWxv44xg\n" \
"W4W44W4px7s2Kpgg6/nNPWs48qzzOxTxh34p1041PzOx2W2W994h\n" \
"-----END RSA PRIVATE KEY-----\n";


void setup() {
	Serial.begin(115200);
	WiFi.begin(ssid, pass);

	net.setCACert((unsigned char*)rootCABuff);
	net.setCertificate((unsigned char*)certificateBuff);
	net.setPrivateKey((unsigned char*)privateKeyBuff);

	client.begin(awsEndPoint, 8883, net);

	connect();
}

void connect() {
	Serial.print("Checking wifi...");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(1000);
	}

	Serial.print("\nconnecting...");
	while (!client.connect("ESP32")) {
		Serial.print(".");
		delay(100);
	}

	Serial.println("\nconnected!");

	client.subscribe(subscribeTopic);
	// client.unsubscribe(subscribeTopic);
}

void loop() {

	if (!client.connected()) {
		connect();
	}
	client.loop();
	delay(100);

	if (millis() - lastMillis > 2000) {   //2 seconds non blocking delay
		lastMillis = millis();
		++value;
		snprintf (msg, 75, "Hello world #%ld", value);
		Serial.print("Publish message: ");
		Serial.println(msg);
		client.publish(publishTopic, msg);
	}
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
	Serial.print("incoming: ");
	Serial.print(topic);
	Serial.print(" - ");
	Serial.print(payload);
	Serial.println();
}
