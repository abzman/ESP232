#if defined(ESP8266)
#include <ESP8266WebServer.h>
ESP8266WebServer webserver(80);
#elif defined(ESP32)
#include <WebServer.h>
WebServer webserver(80);
#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif

void www_setup()
{
  webserver.on("/", handle_OnConnect);
  webserver.on("/set_parm", handle_set_parm);
  webserver.on("/ota", handle_ota);
  webserver.on("/reboot", reboot_esp);
  webserver.onNotFound(handle_NotFound);

  webserver.begin();
  if (!current_config.quiet)
  {
    Serial.println("HTTP server started");
  }
/*
  if (!MDNS.begin(current_config.hostname)) {
    if (!current_config.quiet)
    {
      Serial.println("Error setting up MDNS responder!");
    }
    while (1) {
      delay(1000);
    }
  }
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("telnet", "tcp", 23);
  */
}

bool www_loop()
{
  webserver.handleClient();
  return false;
}

void handle_OnConnect()
{
  webserver.send(200, "text/html", SendHTML());
}

void handle_ota()
{
  ota_setup();
  webserver.send(200, "text/html", SendHTML());
}

void reboot_esp()
{
  webserver.send(200, "text/html", SendHTML());
  ESP.restart();
}

void handle_set_parm()
{
  char hostname[32];
  char ssid[32];
  char password[63];
  strncpy(hostname, webserver.arg("hostname").c_str(), 32);
  strncpy(ssid, webserver.arg("ssid").c_str(), 32);
  strncpy(password, webserver.arg("password").c_str(), 63);

  if (strlen(hostname) > 0 && strlen(hostname) < 31)
  {
    strcpy(current_config.hostname, hostname);
  }
  if (strlen(password) > 7 && strlen(password) < 62)
  {
    strcpy(current_config.password, password);
    if (strlen(ssid) > 0 && strlen(ssid) < 31)
    {
      strcpy(current_config.ssid, ssid);
    }
  }

  int baud = atoi(webserver.arg("baud").c_str());
  current_config.baudrate = max(1200, min(1000000, baud));

  int settings = atoi(webserver.arg("settings").c_str());
  switch (settings) {
    case 1:
      current_config.settings = SERIAL_5N1;
      break;
    case 2:
      current_config.settings = SERIAL_6N1;
      break;
    case 3:
      current_config.settings = SERIAL_7N1;
      break;
    case 4:
      current_config.settings = SERIAL_8N1;
      break;
    case 5:
      current_config.settings = SERIAL_5N2;
      break;
    case 6:
      current_config.settings = SERIAL_6N2;
      break;
    case 7:
      current_config.settings = SERIAL_7N2;
      break;
    case 8:
      current_config.settings = SERIAL_8N2;
      break;
    case 9:
      current_config.settings = SERIAL_5E1;
      break;
    case 10:
      current_config.settings = SERIAL_6E1;
      break;
    case 11:
      current_config.settings = SERIAL_7E1;
      break;
    case 12:
      current_config.settings = SERIAL_8E1;
      break;
    case 13:
      current_config.settings = SERIAL_5E2;
      break;
    case 14:
      current_config.settings = SERIAL_6E2;
      break;
    case 15:
      current_config.settings = SERIAL_7E2;
      break;
    case 16:
      current_config.settings = SERIAL_8E2;
      break;
    case 17:
      current_config.settings = SERIAL_5O1;
      break;
    case 18:
      current_config.settings = SERIAL_6O1;
      break;
    case 19:
      current_config.settings = SERIAL_7O1;
      break;
    case 20:
      current_config.settings = SERIAL_8O1;
      break;
    case 21:
      current_config.settings = SERIAL_5O2;
      break;
    case 22:
      current_config.settings = SERIAL_6O2;
      break;
    case 23:
      current_config.settings = SERIAL_7O2;
      break;
    case 24:
      current_config.settings = SERIAL_8O2;
      break;
    default:
      current_config.settings = SERIAL_8N1;
      break;
  }

  int quiet = atoi(webserver.arg("quiet").c_str());
  current_config.quiet = quiet;

  webserver.send(200, "text/html", SendHTML());
  save_cfg();
  ESP.restart();
}

void handle_NotFound()
{
  webserver.send(404, "text/plain", "Not found");
}

String SendHTML()
{
  char buf[128];

  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";

#if defined(ESP8266)
  sprintf(buf, "<title>ESP8266-232 '%s' Control</title>\n", current_config.hostname);
#elif defined(ESP32)
  sprintf(buf, "<title>ESP232 '%s' Control</title>\n", current_config.hostname);
#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif

  ptr += buf;
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";

#if defined(ESP8266)
  sprintf(buf, "<h1>ESP8266-232 - %s</h1>\n", current_config.hostname);
#elif defined(ESP32)
  sprintf(buf, "<h1>ESP232 - %s</h1>\n", current_config.hostname);
#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif

  ptr += buf;
  if (!ota_enabled)
  {
    ptr += "<a href=\"/ota\">[Enable OTA]</a> ";
  }
  ptr += "<a href=\"/reboot\">[Reboot Device]</a> ";
  if (config_mode)
  {
    ptr += "--- config mode enabled ---";
  }
  ptr += "<br><br>\n";


  ptr += "<form action=\"/set_parm\">\n";
  ptr += "<table>";

  ptr += "<tr><td><label for=\"hostname\">Name:</label></td>";
  sprintf(buf, "<td><input type=\"text\" id=\"hostname\" name=\"hostname\" value=\"%s\"></td></tr>\n", current_config.hostname);
  ptr += buf;
  ptr += "<tr><td><label for=\"baud\">Baud:</label></td>";
  sprintf(buf, "<td><input type=\"text\" id=\"baud\" name=\"baud\" value=\"%d\"></td></tr>\n", current_config.baudrate);
  ptr += buf;
  ptr += "<tr><td><label for=\"SSID\">SSID:</label></td>";
  sprintf(buf, "<td><input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"%s\"></td></tr>\n", current_config.ssid);
  ptr += buf;
  ptr += "<tr><td><label for=\"password\">password:</label></td>";
  sprintf(buf, "<td><input type=\"text\" id=\"password\" name=\"password\" value=\"%s\"></td></tr>\n", current_config.password);
  ptr += buf;
  ptr += "<tr><td><label for=\"settings\">Settings:</label></td>";
  ptr += "<td><select id=\"settings\" name=\"settings\">";
  ptr += "<option value=\"01\"";
  if (current_config.settings == SERIAL_5N1)
    ptr += " selected";
  ptr += ">SERIAL_5N1</option>";
  ptr += "<option value=\"02\"";
  if (current_config.settings == SERIAL_6N1)
    ptr += " selected";
  ptr += ">SERIAL_6N1</option>";
  ptr += "<option value=\"03\"";
  if (current_config.settings == SERIAL_7N1)
    ptr += " selected";
  ptr += ">SERIAL_7N1</option>";
  ptr += "<option value=\"04\"";
  if (current_config.settings == SERIAL_8N1)
    ptr += " selected";
  ptr += ">SERIAL_8N1</option>";
  ptr += "<option value=\"05\"";
  if (current_config.settings == SERIAL_5N2)
    ptr += " selected";
  ptr += ">SERIAL_5N2</option>";
  ptr += "<option value=\"06\"";
  if (current_config.settings == SERIAL_6N2)
    ptr += " selected";
  ptr += ">SERIAL_6N2</option>";
  ptr += "<option value=\"07\"";
  if (current_config.settings == SERIAL_7N2)
    ptr += " selected";
  ptr += ">SERIAL_7N2</option>";
  ptr += "<option value=\"08\"";
  if (current_config.settings == SERIAL_8N2)
    ptr += " selected";
  ptr += ">SERIAL_8N2</option>";
  ptr += "<option value=\"09\"";
  if (current_config.settings == SERIAL_5E1)
    ptr += " selected";
  ptr += ">SERIAL_5E1</option>";
  ptr += "<option value=\"10\"";
  if (current_config.settings == SERIAL_6E1)
    ptr += " selected";
  ptr += ">SERIAL_6E1</option>";
  ptr += "<option value=\"11\"";
  if (current_config.settings == SERIAL_7E1)
    ptr += " selected";
  ptr += ">SERIAL_7E1</option>";
  ptr += "<option value=\"12\"";
  if (current_config.settings == SERIAL_8E1)
    ptr += " selected";
  ptr += ">SERIAL_8E1</option>";
  ptr += "<option value=\"13\"";
  if (current_config.settings == SERIAL_5E2)
    ptr += " selected";
  ptr += ">SERIAL_5E2</option>";
  ptr += "<option value=\"14\"";
  if (current_config.settings == SERIAL_6E2)
    ptr += " selected";
  ptr += ">SERIAL_6E2</option>";
  ptr += "<option value=\"15\"";
  if (current_config.settings == SERIAL_7E2)
    ptr += " selected";
  ptr += ">SERIAL_7E2</option>";
  ptr += "<option value=\"16\"";
  if (current_config.settings == SERIAL_8E2)
    ptr += " selected";
  ptr += ">SERIAL_8E2</option>";
  ptr += "<option value=\"17\"";
  if (current_config.settings == SERIAL_5O1)
    ptr += " selected";
  ptr += ">SERIAL_5O1</option>";
  ptr += "<option value=\"18\"";
  if (current_config.settings == SERIAL_6O1)
    ptr += " selected";
  ptr += ">SERIAL_6O1</option>";
  ptr += "<option value=\"19\"";
  if (current_config.settings == SERIAL_7O1)
    ptr += " selected";
  ptr += ">SERIAL_7O1</option>";
  ptr += "<option value=\"20\"";
  if (current_config.settings == SERIAL_8O1)
    ptr += " selected";
  ptr += ">SERIAL_8O1</option>";
  ptr += "<option value=\"21\"";
  if (current_config.settings == SERIAL_5O2)
    ptr += " selected";
  ptr += ">SERIAL_5O2</option>";
  ptr += "<option value=\"22\"";
  if (current_config.settings == SERIAL_6O2)
    ptr += " selected";
  ptr += ">SERIAL_6O2</option>";
  ptr += "<option value=\"23\"";
  if (current_config.settings == SERIAL_7O2)
    ptr += " selected";
  ptr += ">SERIAL_7O2</option>";
  ptr += "<option value=\"24\"";
  if (current_config.settings == SERIAL_8O2)
    ptr += " selected";
  ptr += ">SERIAL_8O2</option>";
  ptr += "</select></td></tr>\n";

  ptr += "<tr><td><label for=\"quiet\">Suppress serial debug?:</label></td>";
  ptr += "<td><select id=\"quiet\" name=\"quiet\">";
  ptr += "<option value=\"1\"";
  if (current_config.quiet == 1)
    ptr += " selected";
  ptr += ">Yes</option>";
  ptr += "<option value=\"0\"";
  if (current_config.quiet == 0)
    ptr += " selected";
  ptr += ">No</option>";
  ptr += "</select></td></tr>\n";

  ptr += "<td></td><td><input type=\"submit\" value=\"Write\"></td></table></form>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
