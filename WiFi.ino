bool connecting = false;


void wifi_setup()
{
  WiFi.begin(current_config.ssid, current_config.password);
  connecting = true;
}

bool wifi_loop(void)
{
  int status = WiFi.status();
  int curTime = millis();
  static int nextTime = 0;
  static int stateCounter = 0;

  if (nextTime > curTime)
  {
    return false;
  }

  /* standard refresh time */
  nextTime = curTime + 100;

  switch (status)
  {
    case WL_CONNECTED:
      connecting = false;
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
        connecting = false;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        break;
      }
      else
      {
        if (++stateCounter > 50)
        {
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
        
        //failover to create the network it can't find
        WiFi.mode(WIFI_AP);
        WiFi.softAP(current_config.ssid, current_config.password);
        stateCounter = 0;
        break;
      }
  }

  return false;
}
