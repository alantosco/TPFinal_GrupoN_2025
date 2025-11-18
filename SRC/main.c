//Se incluyen todas las librerias a utilizar
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

//Configuracion de pantalla OLED SSD1306 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Configuracion del Hardware
int servoPin = 12;
int sensorPin = 10; // Sensor de ENTRADA (1)
int sensorPinSalida = 11; // Sensor de SALIDA (2)

Servo miServo;
const int SERVO_ARRIBA = 90; //Sensor levantado
const int SERVO_ABAJO = 0; //Sensor abajo

const byte FILAS = 4;
const byte COLS = 4;
char keys[FILAS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinesFilas[FILAS] = {9, 8, 7, 6};
byte pinesCols[COLS] = {5, 4, 3, 2};
Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesCols, FILAS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Configuracion de la clave para ingresar
const char claveSecreta[] = "911"; //Clave de ingreso al estacionamiento
const int LONGITUD_CLAVE = sizeof(claveSecreta) - 1;
char claveIngresada[LONGITUD_CLAVE + 1];
int indiceActual = 0;
bool sistemaActivado = false;

//Variables millis servoLevantado
unsigned long tiempoServoLevantado = 0;
const long duracionServoLevantado = 10000;
bool barreraAbierta = false;

//Variables millis estacionamiento lleno
const int capacidadMaxima = 5;
unsigned long tiempoEstacionamientoLleno = 0;
const long duracionEstacionamientoLleno = 3000;
bool mostrandoLleno = false;

//Variables millis mensaje de error
unsigned long tiempoMensajeError = 0;
const long duracionMensajeError = 3000;
bool mostrandoError = false;

//Variable millis mensaje de activacion
unsigned long tiempoActivacion = 0;
const long duracionMensajeActivacion = 3000; // 3 segundos, reemplaza el delay
bool mostrandoActivacion = false;

//Variable de estado para salida
bool cocheEstaPasando = false;

//Variables millis salida de estacionamiento
bool modoSalidaSinClave = false;  //Para salida de autos
unsigned long tiempoSensor2Salida = 0; //Cuanto tiempo lleva detectando auto en sensor 2
const long tiempoEsperaSensor2 = 3000; 

//Variable que cuenta autos del estacionamiento
long contadorAutos = 0;

void mostrarMensajeBloqueado();
void procesarClave(char tecla);
void controlarBarrera();
void bloquearSistema();
void controlarSalidaSinClave();
void actualizarOLED();
void dibujarRueda(int x, int y, int r);

//Setup inicial
void setup() {
  Serial.begin(9600);

  // --- Inicializar OLED ---
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("ERROR OLED"));
    while (true);
  }
  oled.clearDisplay();
  oled.display();
  actualizarOLED();

  pinMode(sensorPin, INPUT);
  pinMode(sensorPinSalida, INPUT);

  miServo.attach(servoPin);
  miServo.write(SERVO_ABAJO);

  claveIngresada[0] = '\0';

  lcd.init();
  lcd.backlight();

  mostrarMensajeBloqueado();
}

// ---------------- LOOP ---------------------------

void loop() {
  unsigned long tiempoActualGlobal = millis();
  // Manejo de mensaje de error en LCD

  if (mostrandoLleno && (tiempoActualGlobal - tiempoEstacionamientoLleno >= duracionEstacionamientoLleno)) {
        mostrandoLleno = false;
        bloquearSistema();
  }

  if (mostrandoError && (tiempoActualGlobal - tiempoMensajeError >= duracionMensajeError)) {
        mostrandoError = false;
        mostrarMensajeBloqueado();
  }

  if (mostrandoActivacion && (tiempoActualGlobal - tiempoActivacion >= duracionMensajeActivacion)) {
    mostrandoActivacion = false;
    // Prepara la pantalla para el modo de control de barrera.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado Barrera:");
    // NO llama a mostrarMensajeBloqueado, ya que el sistema ya está ACTIVO.
  }

  if (mostrandoLleno || mostrandoError || mostrandoActivacion) {
    return;
  }

  // Lectura del teclado
  char tecla = teclado.getKey();

  if (!mostrandoError && tecla) {
      if (tecla == '*') {
          bloquearSistema();
      } else if (!sistemaActivado) {
          procesarClave(tecla);
      }
  }

  // Si el sistema está activado se sigue la lógica normal de barrera
  if (sistemaActivado) {
      controlarBarrera();
  } else {
      // Si está esperando clave se activa el modo salida sin clave
      controlarSalidaSinClave();
  }
}

// ---------------- FUNCIONES -------------------------

void bloquearSistema() {
    Serial.println("Sistema bloqueado.");
    sistemaActivado = false;
    barreraAbierta = false;
    modoSalidaSinClave = false;
    cocheEstaPasando = false;
    miServo.write(SERVO_ABAJO);
    
    claveIngresada[0] = '\0';
    indiceActual = 0;

    mostrarMensajeBloqueado();
}

void procesarClave(char tecla) {
    if (indiceActual < LONGITUD_CLAVE) {
        claveIngresada[indiceActual] = tecla;
        indiceActual++;
        claveIngresada[indiceActual] = '\0';

        lcd.setCursor(7, 1);
        lcd.print(claveIngresada);
        for (int i = indiceActual; i < LONGITUD_CLAVE; i++) {
            lcd.print(" ");
        }

        if (indiceActual == LONGITUD_CLAVE) {
            if (strcmp(claveIngresada, claveSecreta) == 0) {
                
                Serial.println("Clave Correcta!");
                sistemaActivado = true;

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Sistema Activado");
                lcd.setCursor(0, 1);
                lcd.print("* para bloquear");

                claveIngresada[0] = '\0';
                indiceActual = 0;
                //delay(3000);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Estado Barrera:");

            } else {
                Serial.println("Incorrecto!");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Clave Incorrecta");
                lcd.setCursor(0, 1);
                lcd.print("Error 404");

                tiempoMensajeError = millis();
                mostrandoError = true;

                claveIngresada[0] = '\0';
                indiceActual = 0;
            }
        }
    }
}

// ------------- CONTROL BARRERA (MODO NORMAL) ----------------

void controlarBarrera() {
    int estadoSensorEntrada = digitalRead(sensorPin);
    int estadoSensorSalida = digitalRead(sensorPinSalida);
    unsigned long tiempoActual = millis();
    
    if (barreraAbierta) {
        // Auto sobre sensor 2
        if (estadoSensorSalida == LOW) {
            cocheEstaPasando = true;
            tiempoServoLevantado = tiempoActual;

            lcd.setCursor(0, 1);
            lcd.print("AUTO PASANDO...  ");
            return;
        }

        // El auto terminó de pasar el sensor 2, aumentamos el contador (ingreso un auto)
        if (cocheEstaPasando && estadoSensorSalida == HIGH) {
            Serial.println("Auto pasó sensor 2 (entrada a sistema).");

            contadorAutos++;
            actualizarOLED();

            bloquearSistema();
            return;
        }

        // Timeout de seguridad
        if ((tiempoActual - tiempoServoLevantado) >= duracionServoLevantado) {
            Serial.println("Timeout cerrar (modo normal)");
            bloquearSistema();
            return;
        }

        long tiempoRestante = (duracionServoLevantado - (tiempoActual - tiempoServoLevantado)) / 1000;

        lcd.setCursor(0, 1);
        lcd.print("T: ");
        lcd.print(tiempoRestante);
        lcd.print("s  (ABIERTA) ");
        return;
    }

    // Barrera cerrada, sensor de ENTRADA (1) abre cuando sistema está activado
    if (estadoSensorEntrada == HIGH) {
        if (contadorAutos >= capacidadMaxima) {
            if (!mostrandoLleno) {
                mostrandoLleno = true;
                tiempoEstacionamientoLleno = millis();
                
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("ESTACIONAMIENTO");
                lcd.setCursor(0, 1);
                lcd.print("LLENO!");
            }
            // Mantiene el sistema bloqueado para que la barrera no suba
            //mostrarMensajeBloqueado();
            return; 
        }

        //Si hay lugar en el estacionamiento se abre
        tiempoServoLevantado = tiempoActual;
        barreraAbierta = true;
        cocheEstaPasando = false;
        miServo.write(SERVO_ARRIBA);

        lcd.setCursor(0, 1);
        lcd.print("PASE! (10s)    ");
        Serial.println("Auto en sensor 1 (modo normal)");
    } else {
        lcd.setCursor(0, 1);
        lcd.print("ESPERANDO DETECCION");
    }
}

// ------------- NUEVA FUNCIÓN: SALIDA SIN CLAVE ----------------

void controlarSalidaSinClave() {
    int estadoSensorEntrada = digitalRead(sensorPin);       // sensor 1
    int estadoSensorSalida = digitalRead(sensorPinSalida);  // sensor 2
    unsigned long tiempoActual2 = millis();

    // Si aún NO estamos en modoSalidaSinClave -> mirar sensor 2 por 3 segundos
    if (!modoSalidaSinClave && !barreraAbierta) {

        if (estadoSensorSalida == LOW) {  // LOW = auto presente en sensor 2
            if (tiempoSensor2Salida == 0) {
                tiempoSensor2Salida = tiempoActual2;  // arranca conteo
            } else if (tiempoActual2 - tiempoSensor2Salida >= tiempoEsperaSensor2) {
                // Lleva 3s o más detectado -> abrir barrera para salida sin clave
                Serial.println("Auto en sensor 2 por 3s -> salida sin clave");
                modoSalidaSinClave = true;
                barreraAbierta = true;
                cocheEstaPasando = false;
                tiempoServoLevantado = tiempoActual2;
                miServo.write(SERVO_ARRIBA);

                // Mensaje inicial de apertura y tiempo restante (10s)
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Salida Autorizada"); // Título claro
                lcd.setCursor(0, 1);
                lcd.print("T. Restante: 10s"); // Muestra el valor inicial
            }
        } else {
            // Se perdió la detección antes de llegar a 3s -> reset
            tiempoSensor2Salida = 0;
        }

        return;
    }

    // Ya estamos en modoSalidaSinClave (barrera abierta para que salga)
    if (modoSalidaSinClave && barreraAbierta) {

        // Auto pasa por sensor 1 (entrada) al salir
        if (estadoSensorEntrada == HIGH) {
            cocheEstaPasando = true;
            tiempoServoLevantado = tiempoActual2;  // mantener abierta mientras pasa

            lcd.setCursor(0, 1);
            lcd.print("AUTO SALIENDO   ");
            return;
        }

        // El auto ya dejó de activar el sensor 1 -> terminó de salir
        if (cocheEstaPasando && estadoSensorEntrada == LOW) {
            Serial.println("Auto salio por sensor 1.");

            if (contadorAutos > 0) contadorAutos--; // evitar negativos
            actualizarOLED();

            miServo.write(SERVO_ABAJO);
            barreraAbierta = false;
            modoSalidaSinClave = false;
            cocheEstaPasando = false;
            tiempoSensor2Salida = 0;

            lcd.setCursor(0, 0);
            lcd.print("Ingrese Clave:  ");
            lcd.setCursor(0, 1);
            lcd.print("SALIDA COMPLETA ");
            //delay(1500);
            mostrarMensajeBloqueado();
            return;
        }

        //IF para calcular tiempo de salida
        if (!cocheEstaPasando) { 
            //Calcula los segundos restantes (10s)
            long tiempoRestante = (duracionServoLevantado - (tiempoActual2 - tiempoServoLevantado)) / 1000;
            
            if (tiempoRestante < 0){
              tiempoRestante = 0; 
            }         
            // Refresca la línea 0 para evitar que otros mensajes la pisen
            lcd.setCursor(0, 0); 
            lcd.print("Salida Autorizada");
            
            // Muestra el tiempo restante en la línea 1
            lcd.setCursor(0, 1);
            lcd.print("T. Restante: ");
            lcd.print(tiempoRestante);
            lcd.print("s "); // Espacios para asegurar que el valor anterior se borre
        }

        // Timeout de seguridad
        if (tiempoActual2 - tiempoServoLevantado >= duracionServoLevantado) {
            Serial.println("Timeout cerrar (salida sin clave)");

            miServo.write(SERVO_ABAJO);
            barreraAbierta = false;
            modoSalidaSinClave = false;
            cocheEstaPasando = false;
            tiempoSensor2Salida = 0;

            lcd.setCursor(0, 0);
            lcd.print("Ingrese Clave:  ");
            lcd.setCursor(0, 1);
            lcd.print("TIEMPO AGOTADO  ");
            delay(1500);
            mostrarMensajeBloqueado();
            return;
        }
    }
}

// ---------------- MENSAJE BLOQUEADO ----------------

void mostrarMensajeBloqueado() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ingrese Clave:");
  //lcd.setCursor(0, 1);
  //lcd.print("");
}

//Función para dibujar la rueda en pantalla OLED
void dibujarRueda(int x, int y, int r) {
  oled.drawCircle(x, y, r, SSD1306_WHITE);
  oled.drawCircle(x, y, r - 3, SSD1306_WHITE);
  oled.fillCircle(x, y, 2, SSD1306_WHITE);

  oled.drawLine(x, y - r, x, y + r, SSD1306_WHITE);
  oled.drawLine(x - r, y, x + r, y, SSD1306_WHITE);

  oled.drawLine(x - (r - 3), y - (r - 3), x + (r - 3), y + (r - 3), SSD1306_WHITE);
  oled.drawLine(x - (r - 3), y + (r - 3), x + (r - 3), y - (r - 3), SSD1306_WHITE);
}

//Funcion para actualizar pantalla OLED
void actualizarOLED() {
  oled.clearDisplay();

  dibujarRueda(18, SCREEN_HEIGHT / 2, 12);

  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(40, 0);
  oled.print("CONTADOR:");

  oled.setTextSize(2);
  oled.setCursor(40, 14);
  oled.print(contadorAutos);

  oled.display();
}
