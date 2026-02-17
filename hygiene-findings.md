# Code Hygiene Findings - OnStepX

## Summary
Ran cppcheck static analysis on the codebase. Found several potential issues that need review.

## Critical Findings (Require Review)

### 1. Uninitialized Variables

**Calendars.cpp:68**
```
error: Uninitialized variable: date.valid
return date;
```
- Severity: High
- Impact: Could return garbage data
- Action: Review Calendars.cpp to ensure date.valid is initialized

**Transform.cpp:194**
```
error: Uninitialized variables: mount.r, mount.h, mount.d, mount.a, mount.z, mount.aa1, mount.aa2
return mount;
```
- Severity: High  
- Impact: Coordinate transformation could use uninitialized data
- Action: Review Transform.cpp coordinate initialization logic

**Library.command.cpp:57**
```
warning: Uninitialized variables: *coords.h, *coords.a, *coords.z, *coords.aa1, *coords.aa2
goTo.setGotoTarget(&target);
```
- Severity: Medium
- Impact: Goto target might have uninitialized fields
- Action: Review library command coordinate initialization

### 2. Array Bounds (False Positive)

**Telescope.command.cpp:200**
```
error: Array '"OnStep"[7]' accessed at index 38
sstrcpy(reply, PRODUCT_DESCRIPTION, 40);
```
- Severity: Low (False Positive)
- Impact: None - PRODUCT_DESCRIPTION is a macro, not a 7-char array
- Action: None - cppcheck misunderstood the macro expansion

## Hygiene Status

### Orphaned Files
- ✓ Removed: src/lib/softSpi/Pins.h (truly orphaned)
- ✓ Kept: src/HAL/arduinoM0/ArduinoM0.h (platform definition)

### Bug Fixes Applied
- ✓ Fixed HAL.h to correctly include arduinoM0/ArduinoM0.h instead of non-existent HAL_ZERO.h

### TODO/FIXME Markers
- 35 found (mostly false positives - function names containing "To" like tzToDouble, hmsToDouble)

### Debug Prints
- 5 found in Sample.cpp (intentional - it's a demo plugin)

## Recommendations

1. **High Priority**: Review and fix uninitialized variable issues in:
   - Calendars.cpp
   - Transform.cpp  
   - Library.command.cpp

2. **Medium Priority**: Run full cppcheck scan to completion (took >5 minutes, timed out)

3. **Low Priority**: Consider suppressing false positive TODO warnings for function names

## Next Steps

1. Test compile in Arduino IDE to verify no regressions from HAL.h fix
2. Address uninitialized variable warnings
3. Run full CI pipeline on GitHub Actions for complete analysis
