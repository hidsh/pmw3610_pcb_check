#include <src/PMW3610.h>          // bit-banging driver, thx to: https://github.com/shiranehyuga/PMW3610

#ifdef USE_TINYUSB
  #include <Adafruit_TinyUSB.h>   // for Serial(USB-CDC)
#endif

#define LED         LED_BUILTIN

#define PIN_SDIO    D4
#define PIN_SCLK    D5
#define PIN_NCS     D7
#define PIN_MOTION  D10

PMW3610 sensor(PIN_NCS, PIN_SCLK, PIN_SDIO);
PMW3610_data data;

volatile bool mot_flag = false;

void isr_mot() {
  mot_flag = true;
}

// ----------------------
void setup() {
  pinMode(LED, OUTPUT);

  Serial.begin(115200);
  while (!Serial) delay(10);    // for Serial(USB-CDC)

  Serial.println("--- pmw3610-pcb-check ---");

  if (!sensor.begin()) {
    Serial.println("✗ PMW3610: ERROR");

    while (1) delay(1000);
  }

  Serial.println("✓ PMW3610: OK");

  sensor.setCpi(600);           // mouse speed
  
  pinMode(PIN_MOTION, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_MOTION), isr_mot, FALLING);
}

void loop() {
  if (!mot_flag) return;

  digitalWrite(LED, LOW);       // on
  
  data = sensor.readMotion();   // PMW3610 data capture

  static char s[100] = "";
  sprintf(s, "dx:%4d,  dy:%4d,  Quality:%3d",
              data.dx, data.dy, data.squal);
  Serial.println(s);
  mot_flag = false;

  digitalWrite(LED, HIGH);      // off
}
