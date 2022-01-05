/*
 * Part of Strimer Plus Arduino
 * Version: 2021.10.27
 * Author: Murat TAMCI
 * Web Site: www.themt.co
 * License: Read LICENSE.txt
 */

#include <EEPROM.h>

#define CONFIG_START 32
#define CONFIG_VERSION "spd"

typedef struct {
  char version[3] = CONFIG_VERSION;
  char message[64] = DEFAULT_MSG;
  long color = 0;
  long bg_color = 0;
  unsigned int scrollDelay = DEFAULT_SCROLLDELAY;
  bool invert = DEFAULT_INVERT;
  byte brightness = DEFAULT_BRIGHTNESS;
} Config;

Config config = Config();

bool loadConfig() {
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    {
      for (unsigned int t=0; t<sizeof(config); t++) {
        *((char*)&config + t) = EEPROM.read(CONFIG_START + t);
      }

      return true;
    }

    return false;
}

void saveConfig() {
  for (unsigned int t=0; t<sizeof(config); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&config + t));
}
