bool connecting = false;


void wifi_setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(current_config.ssid, current_config.password);
  //Serial.println(current_config.ssid);
  //Serial.println(current_config.password);

  connecting = true;
}

bool wifi_loop(void)
{
  int status = WiFi.status();
  int curTime = millis();
  static int nextTime = 0;
  static int stateCounter = 0;
  static int retries = 0;

  if (nextTime > curTime)
  {
    return false;
  }
  //Serial.println(status);
  //Serial.println(stateCounter);
  //Serial.println(retries);
  if (retries == 10)
  {

    uint32_t chipId = 0;
#ifdef ESP8266

    chipId = ESP.getChipId();

#endif

#ifdef ESP32

    for (int i = 0; i < 17; i = i + 8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
#endif
    //failover to create a neutral network based on the esp mac
    WiFi.mode(WIFI_AP);
    String stringOne, stringThree;
    stringOne = String("esp232 ");
    stringThree = String();
    stringThree = stringOne  + chipId;
    WiFi.softAP(stringThree);
    Serial.println("blocking ap host");
    ++retries;
    return false;
  }
  if (retries > 10)
  {
    return false;
  }
  /* standard refresh time */
  nextTime = curTime + 100;

  switch (status)
  {
    case WL_CONNECTED:
      connecting = false;
      retries = 0;
      break;

    case WL_CONNECTION_LOST:
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      nextTime = curTime + 500;
      break;

    case WL_CONNECT_FAILED:
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      nextTime = curTime + 1000;
      break;

    case WL_NO_SSID_AVAIL:
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      nextTime = curTime + 2000;
      break;

    case WL_SCAN_COMPLETED:
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      break;

    case WL_DISCONNECTED:
      if (!connecting)
      {
        connecting = true;
        WiFi.mode(WIFI_STA);
        WiFi.begin(current_config.ssid, current_config.password);
        stateCounter = 0;
        break;
      }
      else
      {
        if (++stateCounter > 50)
        {
          ++retries;
          connecting = false;
          WiFi.disconnect();
          WiFi.mode(WIFI_OFF);
        }
      }

    case WL_IDLE_STATUS:
      if (!connecting)
      {
        connecting = true;
        WiFi.mode(WIFI_STA);
        WiFi.begin(current_config.ssid, current_config.password);
        stateCounter = 0;
        break;
      }

    case WL_NO_SHIELD:
      if (!connecting)
      {
        connecting = true;
        //WiFi.mode(WIFI_STA);
        //WiFi.begin(current_config.ssid, current_config.password);

        uint32_t chipId = 0;
#ifdef ESP8266

        chipId = ESP.getChipId();

#endif

#ifdef ESP32

        for (int i = 0; i < 17; i = i + 8) {
          chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
        }
#endif
        //failover to create a neutral network based on the esp mac
        WiFi.mode(WIFI_AP);
        String stringOne, stringThree;
        stringOne = String("esp232 ");
        stringThree = String();
        stringThree = stringOne  + chipId;
        WiFi.softAP(stringThree);
        stateCounter = 0;
        break;
      }
  }

  return false;
}
