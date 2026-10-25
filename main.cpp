#include <AccelStepper.h>

// Pines del hardware
#define STEP_PIN_PAN 26
#define DIR_PIN_PAN 25
#define STEP_PIN_TILT 14
#define DIR_PIN_TILT 27
#define PIN_HIDRO 33

// Límites físicos (ajustables tras el mapeo manual)
#define LIMITE_MAX_PAN 9000
#define LIMITE_MAX_TILT 62
#define MARGEN_SEGURIDAD_PAN 10
#define MARGEN_SEGURIDAD_TILT 20

AccelStepper motorPan(1, STEP_PIN_PAN, DIR_PIN_PAN);
AccelStepper motorTilt(1, STEP_PIN_TILT, DIR_PIN_TILT);

bool enMovimiento = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_HIDRO, OUTPUT);
  digitalWrite(PIN_HIDRO, LOW);
  
  motorPan.setMaxSpeed(1000);
  motorPan.setAcceleration(500);
  
  motorTilt.setMaxSpeed(1000);
  motorTilt.setAcceleration(500);

  Serial.println("READY");
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim(); 
    
    if (comando.length() > 0) {
      int objetivoPan = 0;
      int objetivoTilt = 0;
      int estadoHidro = 0;
      
      if (sscanf(comando.c_str(), "P%d T%d S%d", &objetivoPan, &objetivoTilt, &estadoHidro) == 3) {
        
        digitalWrite(PIN_HIDRO, estadoHidro == 1 ? HIGH : LOW);

        // Calcular los topes dinámicamente según el margen de cada eje
        int minPermitidoPan = MARGEN_SEGURIDAD_PAN;
        int maxPermitidoPan = LIMITE_MAX_PAN - MARGEN_SEGURIDAD_PAN;
        
        int minPermitidoTilt = MARGEN_SEGURIDAD_TILT;
        int maxPermitidoTilt = LIMITE_MAX_TILT - MARGEN_SEGURIDAD_TILT;

        // Limitar la coordenada matemática antes de enviarla al driver
        objetivoPan = constrain(objetivoPan, minPermitidoPan, maxPermitidoPan);
        objetivoTilt = constrain(objetivoTilt, minPermitidoTilt, maxPermitidoTilt);

        motorPan.moveTo(objetivoPan);
        motorTilt.moveTo(objetivoTilt);
        enMovimiento = true;
        
        Serial.println("OK");
      } else {
        Serial.println("ERROR");
      }
    }
  }

  motorPan.run();
  motorTilt.run();

  if (enMovimiento && motorPan.distanceToGo() == 0 && motorTilt.distanceToGo() == 0) {
    Serial.println("DONE");
    enMovimiento = false;
  }
}