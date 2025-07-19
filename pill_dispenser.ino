#define BLYNK_TEMPLATE_ID "TMPL36gcehqNv"
#define BLYNK_TEMPLATE_NAME "Pill Dispenser"

#include <AccelStepper.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEsp8266.h>

// Wi-Fi Credentials
char auth[] = "9e1U02YmFwk6rKPMRvf9JJapLQPVEXso";  
char ssid[] = "Mini";
char pass[] = "hello321";

#define BLYNK_FIRMWARE_VERSION "1.3.2"
#define BLYNK_PRINT Serial
#define APP_DEBUG

// Pin Definitions
#define stepPin D2
#define dirPin D1
#define ledPin D7
#define buzzerPin D5
#define IRSensorPin D6
#define buttonPin D3  // Push button pin

// Set this to true if your buzzer is active-low
#define BUZZER_ACTIVE_LOW true

const float stepsPerRevolution = 200.0;
const int compartments = 4;
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

BlynkTimer timer;
int currentCompartment = 0;
bool notificationSent = false;
bool manualDispensed[compartments] = {false}; // Track manual dispensing

// Rotation times (in hhmm format)
unsigned long rotationTimes[compartments] = {
  1606,
  1608,
  241,
  243
};

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(IRSensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Ensure buzzer is OFF initially
  digitalWrite(buzzerPin, BUZZER_ACTIVE_LOW ? HIGH : LOW);
  digitalWrite(ledPin, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);
  Serial.println("Connected to Blynk!");

  timeClient.begin();
  timeClient.setTimeOffset(19800); // GMT+5:30

  stepper.setMaxSpeed(500);
  stepper.setAcceleration(500);

  timer.setInterval(10000L, checkDispensing); // Every 10s
}

void loop() {
  Blynk.run();
  timer.run();

  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Manual Dispensing Triggered!");
    dispensePill(true);
    delay(1000); // Debounce
  }
}

void checkDispensing() {
  if (!timeClient.update()) {
    timeClient.forceUpdate();
    delay(100);
  }

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  unsigned long currentTime = currentHour * 100 + currentMinute;

  Serial.print("Current Time: ");
  Serial.println(currentTime);
  Serial.print("Expected Time: ");
  Serial.println(rotationTimes[currentCompartment]);

  if (currentTime >= rotationTimes[currentCompartment] &&
      currentTime < rotationTimes[currentCompartment] + 2) {

    if (!manualDispensed[currentCompartment]) {
      Serial.println("Automatic Dispensing...");
      dispensePill(false);
    } else {
      Serial.println("Skipping, already manually dispensed.");
      manualDispensed[currentCompartment] = false;
      currentCompartment = (currentCompartment + 1) % compartments;
    }
  }
}

void dispensePill(bool isManual) {
  Serial.println("Rotating Stepper Motor...");
  int stepsToNextCompartment = stepsPerRevolution / compartments;
  stepper.runToNewPosition(stepper.currentPosition() + stepsToNextCompartment);
  Serial.println("Dispensing complete!");

  digitalWrite(ledPin, HIGH);
  beepBuzzer();  // Buzzer beep pattern

  unsigned long startTime = millis();
  bool handDetected = false;
  notificationSent = false;

  while (millis() - startTime < 5000) {
    int irsensor = digitalRead(IRSensorPin);
    if (irsensor == 0 && !notificationSent) {
      Serial.println("Pill Picked Up!");
      Blynk.logEvent("pill_picked_up", ("Pill " + String(currentCompartment + 1) + " picked up").c_str());
      notificationSent = true;
      handDetected = true;
      break;
    }
    delay(100);
  }

  if (!handDetected) {
    Serial.println("Pill Not Picked!");
    Blynk.logEvent("pill_not_picked", ("Pill " + String(currentCompartment + 1) + " not picked").c_str());
  }

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, BUZZER_ACTIVE_LOW ? HIGH : LOW);

  if (isManual) {
    manualDispensed[currentCompartment] = true;
  }

  currentCompartment = (currentCompartment + 1) % compartments;
}

// Function to beep buzzer 3 times
void beepBuzzer() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, BUZZER_ACTIVE_LOW ? LOW : HIGH);
    delay(200);
    digitalWrite(buzzerPin, BUZZER_ACTIVE_LOW ? HIGH : LOW);
    delay(200);
  }
}

