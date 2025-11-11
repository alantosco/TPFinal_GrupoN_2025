# TPFinal_GrupoN_2025
Repositorio TP FINAL Arduino - LABO 1

1- Vilca Parking XP - Estacionamiento privado con acceso por ID.

2- Vilca Nicolas, Rojas Gabriel, Aguilera Ferreira Maria Belen, Ojeda Alan

3- Pedro Iriso, Matias Gagliardo - Laboratorio de Computacion I 

4- Nuestro Proyecto Arduino, "VILCA PARKING XP" es un estacionamiento privado con deteccion de entrada de autos y acceso mediante ID. 
El sistema identifica la proximidad de un objeto, en este caso un auto a escala, para que una vez cerca de la Barrera, a traves de una pantalla LED se le pide al conductor un codigo de acceso único que introduce por un KeyPad. Si el codigo introducido es correcto abre la barrera y permite el paso. Al detectar que el auto paso en su totalidad, la barrera baja 90 grados. 
En caso de que el codigo ingresado sea incorrecto, la barrera se matiene baja pidiendole que ingrese nuevamente el código. El usuario será informado mediante una pantalla lcd los estados de los intentos de acceso. Luego que el auto pase, la barrera baja y comienza el ciclo repite el proceso para futuros autos. 

5- Cumplimos con el Control de Entradas y Salidas mediante xxx
	Cumplimos con el Contador de flancos mediante xxx
	Cumplimos con el Control logico por maquina de estados mediante xxx
	NO cumplimos con el contador logico por tiempo

6- Componentes: 1 Keypad, 1 Pantalla LCD, 2 Sensores de Proximidad, 1 Servo, 1 Arduino uno, Mini-Protoboard.

7- Esquema electrico
<img width="1197" height="931" alt="Image" src="https://github.com/user-attachments/assets/8e9c10c4-4acb-4ad8-be27-2379b67f4e94" />


8- Maquina de Estados:
	El proyecto posee dos estados:
	Barrera Baja -> El programa queda a la espera de que se aproxime un auto al estacionamiento y pedirle el ID de Acceso, al introducir un ID de Acceso correcto se convierte al estadoo "Barrera Alta"
	Barrera Alta -> El programa espera que el auto pase y luego baja 90 grados convirtiendose en el estado "Barrera Baja"

9- Instrucciones de uso y ejecucion: (GABRIEL)

10- Imagenes o video demostrativo: (ALAN)
