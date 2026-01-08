#pragma once

#include "concurrency/OSThread.h"
#include "PowerStatus.h"
#include "sleep.h"
#include "target_specific.h"

class SolarBatterySoftSleepModule : private concurrency::OSThread
{
public:
    SolarBatterySoftSleepModule();

protected:
    virtual int32_t runOnce() override;

private:
    uint32_t lastCheckTime = 0;
    uint32_t sleepStartTime = 0;

    static constexpr uint32_t LOW_BATTERY_TRIGGER = 15;
    static constexpr uint32_t SAFE_BATTERY_STOP = 30;
    static constexpr uint32_t CHECK_INTERVAL_SECONDS = 60;
    static constexpr uint32_t MIN_SLEEP_MS = 2UL * 60 * 60 * 1000;
    static constexpr uint32_t MAX_SLEEP_CYCLE_MS = 4UL * 60 * 60 * 1000;
};