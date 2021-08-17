
#include <ArduinoOTA.h>

void ota_setup()
{
  if (ota_enabled)
  {
    return;
  }

  ArduinoOTA.setHostname(current_config.hostname);

  ArduinoOTA.onStart([]() {
#if !QUIET_MODE
    Serial.printf("[OTA] active");
#endif
    ota_active = true;
  });
  ArduinoOTA.onEnd([]() {
#if !QUIET_MODE
    Serial.printf("[OTA] inactive");
#endif
    ota_active = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  });
  ArduinoOTA.onError([](ota_error_t error) {
#if !QUIET_MODE
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
#endif
  });
#if !QUIET_MODE
  Serial.printf("[OTA] started\n");
#endif
  ArduinoOTA.begin();
  ota_enabled = true;
}

bool ota_loop()
{
  if (ota_enabled)
  {
    ArduinoOTA.handle();
  }

  return ota_enabled;
}
