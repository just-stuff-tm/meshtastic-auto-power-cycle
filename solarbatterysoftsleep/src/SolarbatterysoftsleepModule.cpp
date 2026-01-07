#include "SolarbatterysoftsleepModule.h"
#include "PowerStatus.h"
#include "sleep.h"

extern meshtastic::PowerStatus *powerStatus;  

SolarBatterySoftSleepModule::SolarBatterySoftSleepModule()
    : concurrency::OSThread("SolarBatterySoftSleep")
{
    
    setIntervalFromNow(CHECK_INTERVAL_SECONDS * 1000);
}

int32_t SolarBatterySoftSleepModule::runOnce()
{
    if (!powerStatus || !powerStatus->getHasBattery()) {
        return CHECK_INTERVAL_SECONDS * 1000;
    }

    uint32_t now = millis();
    uint8_t batteryPercent = powerStatus->getBatteryChargePercent();
    bool isCharging = powerStatus->getIsCharging();

    uint32_t checkIntervalMs = CHECK_INTERVAL_SECONDS * 1000;

    if (sleepStartTime == 0) {
        if (now - lastCheckTime < checkIntervalMs) {
            return checkIntervalMs;
        }
        lastCheckTime = now;

        if (isCharging && batteryPercent <= LOW_BATTERY_TRIGGER) {
            LOG_INFO("Low battery %u%% (<=15%%) + charging → STARTING 2-4hr sleep\n", batteryPercent);
            sleepStartTime = now;

#if defined(NRF52840_XXAA)
            cpuDeepSleep(MIN_SLEEP_MS / 1000);
#else
            powerFSM.goToLightSleep(); 
#endif
        }
        return checkIntervalMs;
    }

    uint32_t elapsed = now - sleepStartTime;

    if (elapsed >= MAX_SLEEP_CYCLE_MS) {
        LOG_INFO("Max 4hr cycle done → awake\n");
        sleepStartTime = 0;
        return checkIntervalMs;
    }

    if (batteryPercent >= SAFE_BATTERY_STOP) {
        LOG_INFO("Battery %u%% (>=30%%) → STOPPING sleep\n", batteryPercent);
        sleepStartTime = 0;
        return checkIntervalMs;
    }

    if (isCharging && batteryPercent < SAFE_BATTERY_STOP) {
        uint32_t remaining = MAX_SLEEP_CYCLE_MS - elapsed;
        uint32_t nextSleep = (remaining > MIN_SLEEP_MS) ? MIN_SLEEP_MS : remaining;

        LOG_INFO("Still low (%u%%) + charging → extend %lum\n", batteryPercent, nextSleep / 60000);

#if defined(NRF52840_XXAA)
        cpuDeepSleep(nextSleep / 1000);
#else
        powerFSM.goToLightSleep();
#endif
    } else {
        LOG_INFO("No charge → end cycle\n");
        sleepStartTime = 0;
    }

    return checkIntervalMs;

}
