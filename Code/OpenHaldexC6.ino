/*
OpenHaldex-C6 - Forbes Automotive (traduit par LVT Technologies)
Contrôleur Haldex pour Gen1, Gen2 et Gen4.  Supporte le WiFi.  Version: 1.06.  Versions dans '_ver.ino'.

Base de code dérivée de OpenHaldex 4.0 - les données CAN restent inchangées, il s'agit seulement d'un port ESP32-C6.
*/

#include <OpenHaldexC6_defs.h>

// pour EEP
Preferences pref;  // pour l'EEPROM / stockage de paramètres

// pour DEL
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(1, gpio_led, led_channel, TYPE_RGB);  // 1 DEL, broche de GPIO, canal, type de DEL

// for mode changing (buttons & external inputs)
InterruptButton btnMode(gpio_mode, HIGH, GPIO_MODE_INPUT, 1000, 500, 750, 80000);          // broche, GPIO_MODE_INPUT, état lorsqu'appuyé, maintenu, auto-répété, double clic, anti-rebond
InterruptButton btnMode_ext(gpio_mode_ext, HIGH, GPIO_MODE_INPUT, 1000, 500, 750, 80000);  // broche, GPIO_MODE_INPUT, état lorsqu'appuyé, maintenu, auto-répété, double clic, anti-rebond

void setup() {
#if enableDebug || detailedDebug || detailedDebugCAN || detailedDebugWiFi || detailedDebugEEP || detailedDebugIO
  Serial.begin(500000);  // SI du série est nécessaire, débuter
  DEBUG("OpenHaldex-C6 Launching...");
#endif

  readEEP();                                                      // lire l'EEPROM pour les paramètres stockés  - dans '_EEP.ino'
  setupIO();                                                      // configurer le gpio pour l'I/O  - dans '_io.ino'
  setupCAN();                                                     // configurer deux bus CAN - dans '_io.ino'
  setupButtons();                                                 // configurer  'buttons' pour le changement de mode (interne et externe) - dans '_io.ino'
  setupTasks();                                                   // configurer les tâches pour chaque fonction principale - gestion CAN Châssis et Haldex, journalisation série, Standalone, etc - dans '_io.ino'
  connectWifi();                                                  // activer / démarrer le WiFi - dans '_wifi.ino'
  setupUI();                                                      // configurer l'interface utilisateur pour le WiFi - dans '_wifi.ino'
}

void loop() {
  delay(100);  // ne sert qu'à donner plus de temps au CPU pour exécuter les tâches
}
