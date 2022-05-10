void handleHelp(char*);
void sendPacket(char*);
void sleepCPU(char*);
void sleepAll(char*);
void sleepLoRa(char*);
void handleCommands(char*);

char msg[256];
double myFreq = 868125000;
uint16_t counter = 0, sf = 12, bw = 125, cr = 0, preamble = 8, txPower = 22;
uint8_t loraBuffer[64];
uint32_t loopBattery = 0, sleepDelay = 10000;
#define batteryDelay 10000

int cmdCount = 0;
struct myCommand {
  void (*ptr)(char *); // Function pointer
  char name[12];
  char help[48];
};

myCommand cmds[] = {
  {sendPacket, "packet", "Sends a packet"},
  {sleepCPU, "cpu", "sleep/cpu"},
  {sleepAll, "all", "sleep/all"},
  {sleepLoRa, "lora", "sleep/lora"},
  {handleHelp, "help", "Shows this help."},
};

void handleHelp(char *param) {
  Serial.printf("Available commands: %d\n", cmdCount);
  for (int i = 0; i < cmdCount; i++) {
    sprintf(msg, " . /%s: %s", cmds[i].name, cmds[i].help);
    Serial.println(msg);
  }
}

void sendPacket(char *param) {
  Serial.printf("Sending... ");
  bool rslt = api.lorawan.psend(64, loraBuffer);
  // when done it will call void send_cb(void);
  Serial.printf("%s\n", rslt ? "Success" : "Fail");
}

void sleepAll(char *param) {
  Serial.printf("Sleep/all for %d secs\n", (sleepDelay / 1000));
  api.system.sleep.all(sleepDelay);
  Serial.println("back");
  // api.system.reboot();
}

void sleepLoRa(char *param) {
  Serial.printf("Sleep/lora for %d secs\n", (sleepDelay / 1000));
  api.system.sleep.lora(sleepDelay);
  Serial.println("back");
  // api.system.reboot();
}

void sleepCPU(char *param) {
  Serial.printf("Sleep/cpu for %d secs\n", (sleepDelay / 1000));
  api.system.sleep.cpu(sleepDelay);
  Serial.println("back");
  // api.system.reboot();
}

void evalCmd(char *str, char *fullString) {
  char strq[12];
  for (int i = 0; i < cmdCount; i++) {
    sprintf(strq, "%s?", cmds[i].name);
    if (strcmp(str, cmds[i].name) == 0 || strcmp(strq, str) == 0) {
      cmds[i].ptr(fullString);
      return;
    }
  } Serial.println("Unknown command");
}

void handleCommands(char *str1) {
  char kwd[32];
  Serial.printf("Incoming: `%s`\n", str1);
  int i = sscanf(str1, "/%s", kwd);
  if (i > 0) evalCmd(kwd, str1);
  else handleHelp("");
}

void handleDelay(char*param) {
  int value;
  int i = sscanf(param, "/%*s %d", &value);
  if (i == -1) {
    // no parameters
    Serial.printf("Current delay: %d s\n", (sleepDelay / 1e3));
    return;
  } else {
    // sf xxxx set SF
    if (value < 1 || value > 30) {
      Serial.printf("Invalid value: %d\n", value);
      return;
    }
    sleepDelay = value * 1000;
  }
}
