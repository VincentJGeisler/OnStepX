// ---------------------------------------------------------------------------------------------------
// Mount status LED and buzzer

#include "Status.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"
#include "../../../lib/gpioEx/GpioEx.h"
#include "../../../lib/nv/Nv.h"

#include "../park/Park.h"

#if STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN != OFF
  bool ledOn = false;
  bool ledOff = false;
  void flash() {
    if (ledOff) { digitalWriteEx(MOUNT_LED_PIN, !MOUNT_LED_ON_STATE); return; }
    if (ledOn) { digitalWriteEx(MOUNT_LED_PIN, MOUNT_LED_ON_STATE); return; }
    static uint8_t cycle = 0;
    if ((cycle++)%2 == 0) {
      digitalWriteEx(MOUNT_LED_PIN, !MOUNT_LED_ON_STATE);
    } else {
      digitalWriteEx(MOUNT_LED_PIN, MOUNT_LED_ON_STATE);
    }
  }
#endif

void generalWrapper() { mountStatus.general(); }

// get mount status ready
void Status::init() {
  if (!nv.hasValidKey()) {
    VLF("MSG: Mount, status writing defaults to NV");
    nv.write(NV_MOUNT_STATUS_BASE, (uint8_t)sound.enabled);
  }

  #if STATUS_BUZZER_MEMORY == ON
    sound.enabled = nv.read(NV_MOUNT_STATUS_BASE);
  #endif

  #if PARK_STATUS != OFF && PARK_STATUS_PIN != OFF
    pinModeEx(PARK_STATUS_PIN, OUTPUT);
  #endif

  VF("MSG: Mount, status start general status task (1s rate priority 4)... ");
  if (tasks.add(1000, 0, true, 4, generalWrapper, "MtStat")) { VLF("success"); } else { VLF("FAILED!"); }
}

// mount status wake on demand
void Status::wake() {
  static bool ready = false;

  if (!ready) {
    #if STATUS_LED == ON
      // if Mount buzzer or status LED are using the Telescope status LED pin, disable it and use here now
      if (STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN == STATUS_LED_PIN) tasks.remove(tasks.getHandleByName("StaLed"));
      if (STATUS_BUZZER != OFF && STATUS_BUZZER_PIN == STATUS_LED_PIN) tasks.remove(tasks.getHandleByName("StaLed"));
    #endif

    #if STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN != OFF
      if (!tasks.getHandleByName("mntLed")) {
        pinModeEx(MOUNT_LED_PIN, OUTPUT);
        VF("MSG: Mount, status start LED task (variable rate priority 4)... ");
        statusTaskHandle = tasks.add(0, 0, true, 4, flash, "mntLed");
        if (statusTaskHandle) { VLF("success"); } else { VLF("FAILED!"); }
      }
    #endif

    #if STATUS_BUZZER != OFF
      VLF("MSG: Mount, status start buzzer");
      sound.init();
    #endif

    ready = true;
  }
}

// mount status LED flash rate (in ms)
void Status::flashRate(int period) {
  #if STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN != OFF
    if (period == 0) { period = 500; ledOff = true; } else ledOff = false;
    if (period == 1) { period = 500; ledOn = true; } else ledOn = false;
    tasks.setPeriod(statusTaskHandle, period/2UL);
  #else
    period = period;
  #endif
}

// mount misc. general status indicators
void Status::general() {
  #if PARK_STATUS != OFF && PARK_STATUS_PIN != OFF
    digitalWriteEx(PARK_STATUS_PIN, (park.state == PS_PARKED) ? PARK_STATUS : !PARK_STATUS)
  #endif
}

// play startup melody
void Status::soundStartupMelody() {
  #if STATUS_BUZZER_STARTUP_MELODY == ON
    #if STATUS_BUZZER != OFF && STATUS_BUZZER_PIN != OFF
      wake(); // ensure buzzer is initialized
      // Only play melody if frequency-based buzzer is configured (STATUS_BUZZER >= 0)
      // Simple on/off buzzer (STATUS_BUZZER == ON) cannot play different frequencies
      #if STATUS_BUZZER >= 0
        // Configurable 3-note startup melody
        // Play regardless of sound.enabled state as it's a startup indicator
        tone(STATUS_BUZZER_PIN, STATUS_BUZZER_MELODY_NOTE1, STATUS_BUZZER_MELODY_DURATION1);
        delay(STATUS_BUZZER_MELODY_DELAY);
        tone(STATUS_BUZZER_PIN, STATUS_BUZZER_MELODY_NOTE2, STATUS_BUZZER_MELODY_DURATION2);
        delay(STATUS_BUZZER_MELODY_DELAY);
        tone(STATUS_BUZZER_PIN, STATUS_BUZZER_MELODY_NOTE3, STATUS_BUZZER_MELODY_DURATION3);
        delay(STATUS_BUZZER_MELODY_DELAY);
        noTone(STATUS_BUZZER_PIN);
      #elif STATUS_BUZZER == ON
        // For simple on/off buzzer, play 3 short beeps instead
        // Play regardless of sound.enabled state as it's a startup indicator
        for (int i = 0; i < 3; i++) {
          digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
          delay(100);
          digitalWriteEx(STATUS_BUZZER_PIN, !STATUS_BUZZER_ON_STATE);
          delay(100);
        }
      #endif
    #endif
  #endif
}

Status mountStatus;

#endif
