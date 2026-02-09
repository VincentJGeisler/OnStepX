OnStepX Telescope Controller
===========================

# What is OnStepX?
OnStepX is the advanced version of the OnStep computerized telescope controller with support for interfacing with/controlling a variety of motor drivers (and so motors) including Step/Dir, ODrive, and Servo (a combination of encoder and DC motor or Stepper motor) types.

It supports:
* Telescope Mount control (Alt/Azm and Equatorial GEM/Fork.)  Optional support for Eq mounts with Tangent Arm Declination.  Usually the Goto capability is enabled, but that's optional as well for those who just want basic mount control.
* Telescope Rotator control (including Alt/Azm de-rotation.)
* Telescope Focuser control (up to 6 focusers so it can handle collimation as well as focusing.)
* Telescope Accessory control (combination of up to 8 dew-heaters, switches, analog PWM.)

# Features
OnStepX supports a wide variety of connection options.  Several serial
"command channels" can be utilized. One of the these is normally devoted to a USB
connection and for the other(s) choose from the following:

* Bluetooth
* ESP8266 WiFi
* Arduino M0/Ethernet Shield
* Even another USB port or RS232 serial isn't very difficult to add.

In the case of running OnStepX on an ESP32 it can provide its own Bluetooth or WiFi IP command channels without additional hardware by simply activating the feature in OnStepX.

Other software in the OnStep ecosystem include:

* an [ASCOM](http://ascom-standards.org/) driver (with IP and Serial support),
* an Android App useable over WiFi or Bluetooth equipped Phones/Tablets,
* a "built-in" website (on the Ethernet and/or WiFi device),
* a full planetarium program that controls all features ([Sky Planetarium](http://stellarjourney.com/index.php?r=site/software_sky)).

OnStep is compatible with the LX200 protocol. This means it can be controlled
from other planetarium software, like: Sky Safari, CdC (even without ASCOM),
Stellarium, etc.

There are also [INDI](http://www.indilib.org/about.html) drivers so it can be used from Linux, with CdC or KStars.

# Documentation
Detailed documentation, including the full set of features, detailed designs for
PCBs, instructions on how to build a controller, how to configure the firmware
for your particular mount, can all be found the [OnStep Group Wiki](https://groups.io/g/onstep/wiki/home).

# Change Log
All the changes are tracking in git, and a detailed list can be accessed using the
following git command:
 
git log --date=short --pretty=format:"%h %ad %<(20)%an %<(150,trunc)%s"

# Support
Questions and discussion should be on the mailing list (also accessible via the
web) at the [OnStep Group](https://groups.io/g/onstep/).

# License
OnStep is open source free software, licensed under the GPL.

See [LICENSE.txt](./LICENSE.txt) file.

# Fork Differences

This fork addresses mechanical limit handling for FORK mounts, which differ significantly from GEM (German Equatorial Mount) behavior. The upstream codebase has a GEM-centric design philosophy that doesn't properly account for FORK mount mechanics.

## Key Differences from Upstream

### FORK Mount Limit Handling
**Problem**: Upstream treats all mount types identically for horizon limits, using altitude (sky-based coordinates) rather than declination (mechanical coordinates). This causes FORK mounts to incorrectly reject valid slew targets and prevents recovery from limit positions.

**Solutions Implemented**:

1. **Mechanical vs. Sky Coordinates** (`Limits.cpp::validateTarget()`)
   - FORK mounts now check declination (mechanical axis position) against horizon limits
   - GEM and ALTAZM mounts continue using altitude (sky-based) as appropriate
   - Prevents false limit violations when FORK mount mechanics allow the position

2. **Directional Limit Enforcement** (`Limits.cpp::poll()`)
   - FORK mounts use directional stopping (like ALTAZM) instead of full-stop behavior
   - Movement TOWARD limits is blocked, movement AWAY from limits is allowed
   - Enables recovery from limit positions without manual intervention

3. **Goto Recovery from Limits** (`Goto.cpp::setTarget()`)
   - Allows goto commands that move back inside limits, even when currently outside
   - Prevents "stuck at limit" scenarios where only manual slew was possible
   - Maintains safety by still rejecting gotos that move further outside limits

### Why This Matters
The upstream's GEM-supremacy attitude assumes all mounts behave like German Equatorials, where:
- Altitude limits make sense (the mount physically can't point below horizon)
- Full-stop at limits is appropriate (meridian flips handle recovery)
- Sky coordinates align with mechanical constraints

FORK mounts are fundamentally different:
- Declination is a mechanical axis that can point "below horizon" when on a wedge
- Directional limits are needed (can always move away from a limit)
- Mechanical coordinates (DEC) matter more than sky coordinates (altitude)

These changes maintain full compatibility with GEM and ALTAZM mounts while properly supporting FORK mount mechanics.

## Upstream Sync Status
This fork periodically merges upstream changes. The FORK mount fixes are designed to coexist with upstream's time-based limit recovery and other enhancements.

Last upstream merge: February 2026 (157 commits merged)

# Author
[Howard Dutton](http://www.stellarjourney.com) (Upstream)

Fork maintained by: [VincentJGeisler](https://github.com/VincentJGeisler) (FORK mount modifications)

