# Meshtastic Solar Battery Soft Sleep Module

A **hard-coded** power conservation plugin designed specifically for **solar-powered Meshtastic nodes**.

When the battery drops to **≤15%** and charging is detected (typically solar input), the node automatically enters extended **light sleep** cycles (2-hour chunks, up to a maximum of 4 hours total) to aggressively conserve power during recovery.

Sleep continues as long as the battery remains below **30%**.  
Once the battery reaches **≥30%**, the node wakes permanently and resumes full normal operation.

This behavior is ideal for off-grid solar deployments: the node stays quiet and power-efficient during low-battery recovery periods and only becomes fully responsive when safely recharged.

### Plugin Tree in project folder
  
    src/modules/
          ├── Modules.cpp
          └── solarbatterysoftsleep/
            └── src/
              ├── SolarbatterysoftsleepModule.h
              ├── SolarbatterysoftsleepModule.cpp
              └── plugin.h
        
(No `.proto` file — intentionally hard-coded with no app configuration.)

### Features

- Triggers long light sleep when battery **≤15%** AND charging detected
- Extends sleep in 2-hour increments while battery < **30%**
- Permanently wakes and stays active when battery **≥30%**
- Runs as efficient background `OSThread` (60-second checks when awake)
- Detailed `LOG_INFO` messages for easy monitoring via serial or BLE
- No user configuration required — fixed, predictable behavior optimized for solar use

### Built For

- Solar-only remote nodes
- Long-term off-grid deployments
- Maximum runtime during low-sun or cloudy conditions

### Compatibility

Testing on **Seeed Wio Tracker L1** (nRF52840) with Meshtastic firmware **2.7.16**.


**<https://Meshforge.org> and <https://registry.meshforge.org>.


### License
MIT
