# Bug Tracking - OnStepX Fork

All bugs are now tracked in GitHub Issues: https://github.com/VincentJGeisler/OnStepX/issues

## Fixed Bugs (Closed Issues)

### Bug #1: Uninitialized variable in Calendars.cpp
**Severity:** High  
**Status:** Fixed (Issue #1)  
**Location:** `src/lib/calendars/Calendars.cpp:68`

**Issue:**
```
error: Uninitialized variable: date.valid
return date;
```

**Impact:**  
Returning uninitialized `date.valid` could cause undefined behavior when checking calendar date validity.

**Resolution:**  
Fixed in commit b8507dcf by initializing `date.valid = true;` before return.

**Found by:** cppcheck static analysis (Code hygiene CI)

---

### Bug #2: Uninitialized mount coordinates in Transform.cpp
**Severity:** High  
**Status:** Fixed (Issue #2)  
**Location:** `src/telescope/mount/coordinates/Transform.cpp:194`

**Issue:**
```
error: Uninitialized variables: mount.r, mount.h, mount.d, mount.a, mount.z, mount.aa1, mount.aa2
return mount;
```

**Impact:**  
Coordinate transformation could use uninitialized data, leading to incorrect telescope positioning or crashes.

**Resolution:**  
Fixed in commit b8507dcf by zero-initializing the structure: `Coordinate mount = {0};`

**Found by:** cppcheck static analysis (Code hygiene CI)

---

### Bug #3: Uninitialized goto target in Library.command.cpp
**Severity:** Medium  
**Status:** Fixed (Issue #3)  
**Location:** `src/telescope/mount/library/Library.command.cpp` (lines 42, 55, 67)

**Issue:**
```
warning: Uninitialized variables: *coords.h, *coords.a, *coords.z, *coords.aa1, *coords.aa2
goTo.setGotoTarget(&target);
```

**Impact:**  
Goto target might have uninitialized coordinate fields, potentially causing incorrect slew operations.

**Resolution:**  
Fixed in commit b8507dcf by zero-initializing all three instances: `Coordinate target = {0};`

**Found by:** cppcheck static analysis (Code hygiene CI)

---

### Bug #4: Missing HAL include for Arduino M0
**Severity:** High  
**Status:** Fixed (Issue #4)  
**Location:** `src/HAL/HAL.h:54`

**Issue:**  
HAL.h referenced non-existent `HAL_ZERO.h` instead of `arduinoM0/ArduinoM0.h`

**Impact:**  
Arduino M0 (SAMD21) builds would fail with missing include error.

**Fix:**  
Changed include from `HAL_ZERO.h` to `arduinoM0/ArduinoM0.h`

**Found by:** Manual code review during hygiene checks

---

## Notes

- ✓ GitHub Issues enabled and tracking bugs
- All fixes committed to ci-hygiene-testing branch (commits e4db2b68 and b8507dcf)
- These bugs exist in upstream (hjd1964/OnStepX) as well - consider reporting upstream
- Issue #111 was accidentally created in upstream repo (hjd1964/OnStepX) - same as our Issue #1
