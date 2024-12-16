#include <ModbusMaster.h>

const int ledPin = 11;         // Pin del LED
const int buttonPin = 4;       // Pin del botón

bool ledOn = False;            // Estado del LED

// Crear una instancia del maestro Modbus
ModbusMaster node;

void setup() {
  // Configuración de pines
  pinMode(ledPin, OUTPUT);        // Configura el pin del LED como salida
  pinMode(buttonPin, INPUT_PULLUP); // Configura el pin del botón como entrada con resistencia pull-up

  // Inicialización del puerto serie y Modbus
  Serial.begin(9600);            // Comunicación con TeslaSCADA
  node.begin(2, Serial);         // ID del esclavo Modbus (cambiado a 2)

  Serial.println("Sistema iniciado");
}

void loop() {
  // Lee el estado del botón
  int buttonState = digitalRead(buttonPin);

  // Cambia el estado del LED si se detecta un pulso del botón
  if (buttonState == LOW) { // Botón presionado (con INPUT_PULLUP, LOW es presionado)
    ledOn = !ledOn; // Alterna el estado
    delay(200);     // Retraso para evitar rebotes
  }

  // Realiza el efecto de brillo si el LED está encendido
  if (ledOn) {
    // Efecto de brillo del LED
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(ledPin, brightness); // Aumenta la intensidad del LED
      delay(10);
    }
    for (int brightness = 255; brightness >= 0; brightness--) {
      analogWrite(ledPin, brightness); // Disminuye la intensidad del LED
      delay(10);
    }
  } else {
    analogWrite(ledPin, 0); // Apaga el LED
  }

  // Comunicación con TeslaSCADA
  enviarEstadoSCADA(ledOn); // Enviar estado del LED al SCADA
  recibirComandoSCADA();    // Recibir comando del SCADA para encender/apagar el LED
}

void enviarEstadoSCADA(bool estado) {
  uint16_t valor = estado ? 1 : 0; // 1 si el LED está encendido, 0 si está apagado

  uint8_t result = node.writeSingleRegister(0x0000, valor); // Escribe en el registro 0x0000

  // Manejo de errores
  if (result == node.ku8MBSuccess) {
    Serial.println("Estado enviado correctamente al SCADA");
  } else {
    Serial.print("Error al enviar estado al SCADA. Código: ");
    Serial.println(result);
  }
}

void recibirComandoSCADA() {
  uint8_t result = node.readHoldingRegisters(0x0001, 1); // Lee el registro 0x0001

  // Manejo de errores
  if (result == node.ku8MBSuccess) {
    uint16_t comando = node.getResponseBuffer(0); // Obtén el comando del registro

    if (comando == 1) {
      ledOn = true; // Encender el LED
      Serial.println("Comando recibido: Encender LED");
    } else if (comando == 0) {
      ledOn = false; // Apagar el LED
      Serial.println("Comando recibido: Apagar LED");
    } else {
      Serial.println("Comando desconocido recibido");
    }
  } else {
    Serial.print("Error al leer comando del SCADA. Código: ");
    Serial.println(result);
  }
}

