# Bug Tracking - OnStepX Fork

## Critical Bugs (High Priority)

### Bug #1: Uninitialized variable in Calendars.cpp
**Severity:** High  
**Status:** Open  
**Location:** `src/lib/calendars/Calendars.cpp:68`

**Issue:**
```
error: Uninitialized variable: date.valid
return date;
```

**Impact:**  
Returning uninitialized `date.valid` could cause undefined behavior when checking calendar date validity.

**Action Required:**  
Review Calendars.cpp to ensure `date.valid` is properly initialized before return.

**Found by:** cppcheck static analysis (Code hygiene CI)

---

### Bug #2: Uninitialized mount coordinates in Transform.cpp
**Severity:** High  
**Status:** Open  
**Location:** `src/telescope/mount/coordinates/Transform.cpp:194`

**Issue:**
```
error: Uninitialized variables: mount.r, mount.h, mount.d, mount.a, mount.z, mount.aa1, mount.aa2
return mount;
```

**Impact:**  
Coordinate transformation could use uninitialized data, leading to incorrect telescope positioning or crashes.

**Action Required:**  
Review Transform.cpp coordinate initialization logic. Ensure all mount structure fields are initialized before return, especially in edge cases.

**Found by:** cppcheck static analysis (Code hygiene CI)

---

### Bug #3: Uninitialized goto target in Library.command.cpp
**Severity:** Medium  
**Status:** Open  
**Location:** `src/telescope/mount/library/Library.command.cpp:57`

**Issue:**
```
warning: Uninitialized variables: *coords.h, *coords.a, *coords.z, *coords.aa1, *coords.aa2
goTo.setGotoTarget(&target);
```

**Impact:**  
Goto target might have uninitialized coordinate fields, potentially causing incorrect slew operations.

**Action Required:**  
Review library command coordinate initialization. Ensure target structure is fully initialized before passing to setGotoTarget().

**Found by:** cppcheck static analysis (Code hygiene CI)

---

## Fixed Bugs

### Bug #4: Missing HAL include for Arduino M0
**Severity:** High  
**Status:** Fixed (commit e4db2b68)  
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

- To enable GitHub Issues on this fork, go to Settings → Features → Issues
- These bugs exist in upstream (hjd1964/OnStepX) as well
- Consider reporting critical bugs upstream after verification
