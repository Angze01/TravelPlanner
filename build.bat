@echo off
echo ========================================================
echo   Travel Planner - Compiling...
echo ========================================================

g++ -std=c++17 ^
    -I src/core -I src/activities -I src/ui -I src/utils ^
    -o TravelPlanner.exe ^
    src/core/Activity.cpp src/core/Day.cpp src/core/Trip.cpp ^
    src/activities/Attraction.cpp src/activities/Restaurant.cpp ^
    src/activities/Hotel.cpp src/activities/Transport.cpp ^
    src/utils/FileManager.cpp src/ui/TravelApp.cpp src/main.cpp

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Compilation finished! Run .\TravelPlanner.exe
) else (
    echo.
    echo [ERROR] Compilation failed.
)
pause
