# Smart Pill Dispenser using ESP8266 & Blynk

An IoT-based automatic pill dispenser that uses an ESP8266, stepper motor, and Blynk app to dispense medicine at 4 user-defined schedules. Includes IR sensor to detect pill pickup and alerts via buzzer, LED, and Blynk notifications.

## 🚀 Features
- User-defined pill schedule via Blynk app
- Stepper motor-driven dispensing
- IR sensor detection for pill pickup
- Notifications and alerts

## 🔧 Hardware Used
- ESP8266 (NodeMCU)
- NEMA 17 Stepper Motor + A3967 Driver
- IR Sensor
- Buzzer & LED
- 12V DC Adapter

## 📱 Blynk App Setup
- Add 4 Time Input widgets (schedule 1 to 4)
- Add Virtual pins V1–V4 for time, V5 for status display
- Add Eventor for actions
- Link to your Blynk Auth Token and Wi-Fi credentials in code

## 📂 Files Included
- pill_dispenser.ino — Main Arduino code
- Circuit diagram
- notification screenshots via Email and Blynk
