# Meshtastic Auto Power Cycle Module

Automatically shuts down device on low battery and resumes when sufficiently recharged.  
Sends optional status messages on LongFast channel (or DM).

### Plugin Tree
  
    plugin/
      └── auto-power-cycle/
          ├── plugin.yaml
          └── src/
              ├── AutoPowerCycleModule.h
              ├── AutoPowerCycleModule.cpp   (Includes ModuleRegistry)
              └── mesh.proto                 
        
### Features
- Configurable low battery threshold (default 20%)
- Configurable resume threshold (default 60%)
- Silent periodic battery checks
- One message on shutdown, one on power-up
- Default target: LongFast channel
- Fully configurable via Meshtastic app

### Flash Firmware
https://flasher.meshtastic.org/

### License
MIT
