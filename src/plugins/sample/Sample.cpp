// Sample plugin

#include "Sample.h"
#include "../../Common.h"
#include "../../libApp/commands/CommandBroker.h"
#include "../../lib/tasks/OnTask.h"

void sampleWrapper() { sample.loop(); }

void Sample::init() {
  VLF("MSG: Plugins, starting: sample");

  // start a task that runs twice a second, run at priority level 7 so
  // we can block using tasks.yield(); fairly aggressively without significant impact on operation
  tasks.add(500, 0, true, 7, sampleWrapper);
}

void Sample::loop() {
  static uint8_t raRequest = 0;
  static uint8_t decRequest = 0;
  char reply[40];

  if (!raRequest) raRequest = commandBroker.request(":GR#");

  switch (commandBroker.result(raRequest, reply, sizeof(reply))) {
    case CB_DONE:
      Serial.print("RA = ");
      Serial.println(reply);
    break;
    case CB_TIMEOUT:
      Serial.println("RA = ?");
    break;
    default:
    break;
  }

  if (!decRequest) decRequest = commandBroker.request(":GD#");

  switch (commandBroker.result(decRequest, reply, sizeof(reply))) {
    case CB_DONE:
      Serial.print("Dec=");
      Serial.println(reply);
    break;
    case CB_TIMEOUT:
      Serial.println("Dec=?");
    break;
    default:
    break;
  }
}

// no command processing in this example, so just return false (not handled)
bool Sample::command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(*reply);
  UNUSED(*command);
  UNUSED(*parameter);
  UNUSED(*suppressFrame);
  UNUSED(*numericReply);
  UNUSED(*commandError);
  return false;
}

Sample sample;
