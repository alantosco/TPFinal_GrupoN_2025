# TPFinal_GrupoN_2025
Repositorio TP FINAL Arduino - LABO 1

1- Vilca Parking XP - Estacionamiento privado con acceso por contraseña.

2- Vilca Nicolas, Rojas Gabriel, Aguilera Ferreira Maria Belen, Ojeda Alan

3- Pedro Iriso, Matias Gagliardo - Laboratorio de Computacion I 

4- Nuestro Proyecto Arduino, "VILCA PARKING XP" es un estacionamiento privado con deteccion de entrada y salida de vehiculos. 
El sistema identifica la proximidad de un objeto, en este caso un auto a escala, para que una vez cerca de la Barrera, a traves de una pantalla LED se le pide al conductor un codigo de acceso único que introduce por un KeyPad. Si el codigo introducido es correcto abre la barrera y permite el paso. Al detectar que el auto paso en su totalidad, la barrera baja 90 grados. 
En caso de que el codigo ingresado sea incorrecto, la barrera se matiene baja pidiendole que ingrese nuevamente el código. El usuario será informado mediante una pantalla lcd los estados de los intentos de acceso. Luego que el vehiculo pase, la barrera baja y comienza el ciclo repite el proceso para futuros autos. Al salir del estacionamiento, el vehiculo es detectado por el segundo sensor, se procede a abrir la barrera y baja cuando haya salido en su totalidad.

5- En el proyecto implementamos un Control de Entradas y Salidas mediante el uso de dos sensores infrarrojos estratégicamente ubicados, permitiendo identificar el sentido de circulación de los vehículo. Además, se cumple con el Contador de Flancos, ya que el sistema suma o resta el contador de autos únicamente al detectar las transiciones de presencia y ausencia en los sensores. También se cumple con el Control Lógico basado en una Máquina de Estados, organizando el funcionamiento del sistema en estados como: bloqueo, ingreso autorizado, salida sin clave, barrera abierta y barrera cerrada, asegurando un comportamiento ordenado y hasta predecible. Finalmente, se cumple con el Contador Lógico por Tiempo, utilizando la función millis() para gestionar temporizaciones como el tiempo de apertura de la barrera, mensajes en pantalla y detección prolongada de vehículos.

6- Componentes: 1 Keypad, 1 Pantalla LCD, 2 Sensores de Proximidad, 1 Servo, 1 Arduino uno, Mini-Protoboard.

7- Esquema electrico
<img width="1197" height="931" alt="Image" src="https://github.com/user-attachments/assets/8e9c10c4-4acb-4ad8-be27-2379b67f4e94" />


8- Maquina de Estados:
	El proyecto posee dos estados:
	Barrera Baja -> El programa queda a la espera de que se aproxime un auto al estacionamiento y pedirle el ID de Acceso, al introducir un ID de Acceso correcto se convierte al estado "Barrera Alta"
	Barrera Alta -> El programa espera que el auto pase y luego baja 90 grados convirtiendose en el estado "Barrera Baja"

9- Instrucciones de uso y ejecucion: (GABRIEL)

10- Imagenes o video demostrativo: (ALAN)

11- Licencia y Creditos:
	Creditos a nuestro docente, Pedro Iriso, que nos presto de suma urgencia un Arduino UNO y un CABLE USB un domingo a las 5 de la tarde
