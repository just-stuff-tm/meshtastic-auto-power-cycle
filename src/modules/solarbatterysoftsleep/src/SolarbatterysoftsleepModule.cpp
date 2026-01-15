#include "SolarbatterysoftsleepModule.h"
#include "PowerStatus.h"
#include "sleep.h"

extern meshtastic::PowerStatus *powerStatus;

SolarBatterySoftSleepModule::SolarBatterySoftSleepModule()
    : concurrency::OSThread("SolarBatterySoftSleep")
{
    LOG_DEBUG("[SolarSoftSleep] Module constructed and started\n");
    setIntervalFromNow(CHECK_INTERVAL_SECONDS * 1000);
}

int32_t SolarBatterySoftSleepModule::runOnce()
{
    // Early exit if no battery or powerStatus not ready
    if (!powerStatus || !powerStatus->getHasBattery()) {
        LOG_DEBUG("[SolarSoftSleep] No battery or powerStatus unavailable → skipping check\n");
        return CHECK_INTERVAL_SECONDS * 1000;
    }

    uint32_t now = millis();
    uint8_t batteryPercent = powerStatus->getBatteryChargePercent();
    bool isCharging = powerStatus->getIsCharging();

    LOG_DEBUG("[SolarSoftSleep] Check triggered | Battery: %u%%\n", batteryPercent);

    uint32_t checkIntervalMs = CHECK_INTERVAL_SECONDS * 1000;

    // ========================================
    // Not yet in sleep mode
    // ========================================
    if (sleepStartTime == 0) {
        if (now - lastCheckTime < checkIntervalMs) {
            LOG_DEBUG("[SolarSoftSleep] Rate-limited → skipping this cycle\n");
            return checkIntervalMs;
        }
        lastCheckTime = now;

        if (batteryPercent <= LOW_BATTERY_TRIGGER) {
            LOG_INFO("[SolarSoftSleep] TRIGGER: Low battery %u%% (<=15%%) STARTING soft sleep cycle\n", batteryPercent);
            sleepStartTime = now;
#if defined(NRF52840_XXAA)
            LOG_DEBUG("[SolarSoftSleep] Entering CPU deep sleep for initial period\n");
            cpuDeepSleep(MIN_SLEEP_MS / 1000);
#else
            LOG_DEBUG("[SolarSoftSleep] Entering light sleep via powerFSM\n");
            powerFSM.goToLightSleep();
#endif
        } else {
            LOG_DEBUG("[SolarSoftSleep] Normal operation: Battery %u%%, no sleep needed\n", batteryPercent);
        }
        return checkIntervalMs;
    }

    // ========================================
    // Currently in a sleep cycle
    // ========================================
    uint32_t elapsed = now - sleepStartTime;
    LOG_DEBUG("[SolarSoftSleep] In sleep cycle | Elapsed: %lu min | Total max: %lu min\n",
              elapsed / 60000, MAX_SLEEP_CYCLE_MS / 60000);

    // Max 4-hour cycle reached
    if (elapsed >= MAX_SLEEP_CYCLE_MS) {
        LOG_INFO("[SolarSoftSleep] Max sleep cycle (4hr) reached → WAKING UP permanently\n");
        sleepStartTime = 0;
        return checkIntervalMs;
    }

    // Battery recovered enough
    if (batteryPercent >= SAFE_BATTERY_STOP) {
        LOG_INFO("[SolarSoftSleep] Battery recovered to %u%% (>=30%%) → STOPPING sleep cycle\n", batteryPercent);
        sleepStartTime = 0;
        return checkIntervalMs;
    }

    // Battery still low → extend sleep
    if (batteryPercent < SAFE_BATTERY_STOP) {
        uint32_t remaining = MAX_SLEEP_CYCLE_MS - elapsed;
        uint32_t nextSleep = (remaining > MIN_SLEEP_MS) ? MIN_SLEEP_MS : remaining;

        LOG_INFO("[SolarSoftSleep] Battery still low (%u%%) → extending sleep by %lu min\n",
                 batteryPercent, nextSleep / 60000);

#if defined(NRF52840_XXAA)
        LOG_DEBUG("[SolarSoftSleep] Entering CPU deep sleep for %lu seconds\n", nextSleep / 1000);
        cpuDeepSleep(nextSleep / 1000);
#else
        LOG_DEBUG("[SolarSoftSleep] Entering light sleep via powerFSM\n");
        powerFSM.goToLightSleep();
#endif
    } else {
        // Battery recovered → end cycle
        LOG_INFO("[SolarSoftSleep] Battery recovered (%u%%) → ENDING sleep cycle\n", batteryPercent);
        sleepStartTime = 0;
    }

    return checkIntervalMs;
}
