# Local Code Hygiene Check Script
# Runs the same checks as CI pipeline locally on Windows

Write-Host "=== OnStepX Code Hygiene Checks ===" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Continue"
$hasErrors = $false

# Check if cppcheck is installed
Write-Host "Checking for cppcheck..." -ForegroundColor Yellow
if (Get-Command cppcheck -ErrorAction SilentlyContinue) {
    Write-Host "✓ cppcheck found" -ForegroundColor Green
} else {
    Write-Host "✗ cppcheck not found" -ForegroundColor Red
    Write-Host "  Install from: https://github.com/danmar/cppcheck/releases" -ForegroundColor Gray
    Write-Host "  Or use: choco install cppcheck" -ForegroundColor Gray
    $hasErrors = $true
}

Write-Host ""
Write-Host "=== 1. Dead Code Detection ===" -ForegroundColor Cyan

if (Get-Command cppcheck -ErrorAction SilentlyContinue) {
    Write-Host "Checking for unused functions..."
    cppcheck --enable=unusedFunction `
        --suppress=missingIncludeSystem `
        --inline-suppr `
        --error-exitcode=0 `
        --std=c++11 `
        -I src `
        -I src/HAL `
        -I src/lib `
        src/ 2>&1 | Tee-Object -FilePath "hygiene-unused-functions.txt"
    
    if (Select-String -Path "hygiene-unused-functions.txt" -Pattern "unusedFunction" -Quiet) {
        Write-Host "⚠️  Found unused functions (see hygiene-unused-functions.txt)" -ForegroundColor Yellow
    } else {
        Write-Host "✓ No unused functions detected" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "Checking for orphaned header files..."
$orphanedHeaders = @()
Get-ChildItem -Path src -Recurse -Filter "*.h" | ForEach-Object {
    $header = $_
    $cppFile = $header.FullName -replace '\.h$', '.cpp'
    
    if (-not (Test-Path $cppFile)) {
        $headerName = $header.Name
        $includeCount = (Get-ChildItem -Path . -Recurse -Include "*.cpp","*.h","*.ino" | 
            Select-String -Pattern $headerName -SimpleMatch | 
            Measure-Object).Count
        
        if ($includeCount -eq 0) {
            $orphanedHeaders += $header.FullName
        }
    }
}

if ($orphanedHeaders.Count -gt 0) {
    Write-Host "⚠️  Found $($orphanedHeaders.Count) potentially orphaned headers:" -ForegroundColor Yellow
    $orphanedHeaders | ForEach-Object { Write-Host "  - $_" -ForegroundColor Gray }
} else {
    Write-Host "✓ No orphaned headers detected" -ForegroundColor Green
}

Write-Host ""
Write-Host "=== 2. Duplicate Code Detection ===" -ForegroundColor Cyan
Write-Host "⚠️  PMD CPD requires Java - skipping duplicate detection" -ForegroundColor Yellow
Write-Host "  (This check runs in CI pipeline)" -ForegroundColor Gray

Write-Host ""
Write-Host "=== 3. Static Analysis ===" -ForegroundColor Cyan

if (Get-Command cppcheck -ErrorAction SilentlyContinue) {
    Write-Host "Running cppcheck static analysis..."
    cppcheck --enable=warning,style,performance,portability `
        --suppress=missingIncludeSystem `
        --inline-suppr `
        --error-exitcode=0 `
        --std=c++11 `
        --platform=unix32 `
        -I src `
        -I src/HAL `
        -I src/lib `
        src/ 2>&1 | Tee-Object -FilePath "hygiene-cppcheck.txt"
    
    $warningCount = (Select-String -Path "hygiene-cppcheck.txt" -Pattern "\[.*\]" | Measure-Object).Count
    if ($warningCount -gt 0) {
        Write-Host "⚠️  Found $warningCount warnings (see hygiene-cppcheck.txt)" -ForegroundColor Yellow
    } else {
        Write-Host "✓ No warnings detected" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "Checking for empty catch blocks..."
$emptyCatches = Get-ChildItem -Path src -Recurse -Filter "*.cpp" | 
    Select-String -Pattern "catch.*\{[\s\r\n]*\}" -AllMatches

if ($emptyCatches) {
    Write-Host "⚠️  Found potential empty catch blocks:" -ForegroundColor Yellow
    $emptyCatches | ForEach-Object { 
        Write-Host "  - $($_.Filename):$($_.LineNumber)" -ForegroundColor Gray 
    }
} else {
    Write-Host "✓ No empty catch blocks detected" -ForegroundColor Green
}

Write-Host ""
Write-Host "=== 4. Configuration Drift ===" -ForegroundColor Cyan

Write-Host "Checking for TODO/FIXME markers..."
$todos = Get-ChildItem -Path src -Recurse -Include "*.cpp","*.h","*.ino" | 
    Select-String -Pattern "(TODO|FIXME|HACK|XXX)" -AllMatches

if ($todos) {
    $todoCount = ($todos | Measure-Object).Count
    Write-Host "⚠️  Found $todoCount TODO/FIXME markers" -ForegroundColor Yellow
    $todos | Select-Object -First 10 | ForEach-Object {
        Write-Host "  - $($_.Filename):$($_.LineNumber): $($_.Line.Trim())" -ForegroundColor Gray
    }
    if ($todoCount -gt 10) {
        Write-Host "  ... and $($todoCount - 10) more" -ForegroundColor Gray
    }
} else {
    Write-Host "✓ No TODO markers found" -ForegroundColor Green
}

Write-Host ""
Write-Host "Checking for debug print statements..."
$debugPrints = Get-ChildItem -Path src -Recurse -Filter "*.cpp" | 
    Select-String -Pattern "(Serial\.print|printf.*debug|DEBUG_PRINT)" -AllMatches

if ($debugPrints) {
    $debugCount = ($debugPrints | Measure-Object).Count
    Write-Host "⚠️  Found $debugCount debug print statements" -ForegroundColor Yellow
    $debugPrints | Select-Object -First 5 | ForEach-Object {
        Write-Host "  - $($_.Filename):$($_.LineNumber)" -ForegroundColor Gray
    }
    if ($debugCount -gt 5) {
        Write-Host "  ... and $($debugCount - 5) more" -ForegroundColor Gray
    }
} else {
    Write-Host "✓ No debug prints found" -ForegroundColor Green
}

Write-Host ""
Write-Host "=== Summary ===" -ForegroundColor Cyan
Write-Host "Reports generated:" -ForegroundColor White
if (Test-Path "hygiene-unused-functions.txt") {
    Write-Host "  - hygiene-unused-functions.txt" -ForegroundColor Gray
}
if (Test-Path "hygiene-cppcheck.txt") {
    Write-Host "  - hygiene-cppcheck.txt" -ForegroundColor Gray
}

Write-Host ""
if ($hasErrors) {
    Write-Host "⚠️  Some checks could not run due to missing tools" -ForegroundColor Yellow
} else {
    Write-Host "✓ All hygiene checks completed" -ForegroundColor Green
}

Write-Host ""
Write-Host "To install missing tools:" -ForegroundColor White
Write-Host "  choco install cppcheck" -ForegroundColor Gray
