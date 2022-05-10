#include "Commands.h"

void send_cb(void) {
  // TX callback
  Serial.println("Tx done!");
}

void setup() {
  Serial.begin(115200, RAK_CUSTOM_MODE);
  // RAK_CUSTOM_MODE disables AT firmware parsing
  time_t timeout = millis();
  while (!Serial) {
    // on nRF52840, Serial is not available right away.
    // make the MCU wait a little
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }
  uint8_t x = 5;
  while (x > 0) {
    Serial.printf(" % d, ", x--);
    delay(500);
  } // Just for show
  Serial.println("0!");
  Serial.println("RUI3 test");
  Serial.println("------------------------------------------------------");
  char HardwareID[16]; // nrf52840
  strcpy(HardwareID, api.system.chipId.get().c_str());
  Serial.printf("Hardware ID: %s\n", HardwareID);
  if (strcmp(HardwareID, "nrf52840") == 0) {
    Serial.println("BLE compatible!");
  }
  sprintf(msg, "* Model ID: %s", api.system.modelId.get().c_str());
  Serial.println(msg);
  Serial.printf("* RUI API Version: %s\n", api.system.apiVersion.get().c_str());
  Serial.printf("* Firmware Version: %s\n", api.system.firmwareVersion.get().c_str());
  Serial.printf("* AT version: %s\n", api.system.cliVersion.get().c_str());

  // LoRa setup – everything else has been done for you. No need to fiddle with pins, etc
  Serial.printf("Set work mode to P2P: %s\n", api.lorawan.nwm.set(0) ? "Success" : "Fail");
  Serial.printf("Set P2P frequency to %3.3f: %s\n", (myFreq / 1e6), api.lorawan.pfreq.set(myFreq) ? "Success" : "Fail");
  Serial.printf("Set P2P spreading factor to %d: %s\n", sf, api.lorawan.psf.set(sf) ? "Success" : "Fail");
  Serial.printf("Set P2P bandwidth to %d: %s\n", bw, api.lorawan.pbw.set(bw) ? "Success" : "Fail");
  Serial.printf("Set P2P coding rate to 4/%d: %s\n", (cr + 5), api.lorawan.pcr.set(cr) ? "Success" : "Fail");
  Serial.printf("Set P2P preamble length to %d: %s\n", preamble, api.lorawan.ppl.set(preamble) ? "Success" : "Fail");
  Serial.printf("Set P2P TX power to %d: %s\n", txPower, api.lorawan.ptp.set(txPower) ? "Success" : "Fail");
  // SX126xWriteRegister(0x08e7, OCP_value);
  // Serial.printf("Set OCP to 0x%2x [%d]\n", OCP_value, (OCP_value * 2.5));
  // LoRa callbacks
  //api.lorawan.registerPRecvCallback(recv_cb);
  api.lorawan.registerPSendCallback(send_cb);
  for (uint8_t ix = 0; ix < 64; ix++) loraBuffer[ix] = ix;
  cmdCount = sizeof(cmds) / sizeof(myCommand);
  handleHelp("");
}

void loop() {
  if (millis() - loopBattery > batteryDelay) {
    Serial.printf("Battery level: %.3f\n", api.system.bat.get());
    loopBattery = millis();
  }

  if (Serial.available()) {
    char str1[256];
    str1[0] = '>';
    str1[1] = ' ';
    uint8_t ix = 2;
    while (Serial.available()) {
      char c = Serial.read();
      if (c > 31) str1[ix++] = c;
    }
    str1[ix] = 0;
    Serial.println(str1);
    handleCommands(str1 + 2);
  }
}
