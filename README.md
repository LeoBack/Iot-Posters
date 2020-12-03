# Iot-Posters
Proyecto de Iot para control de cartelería publica.

Basado en NodeMCU ESP8266 utilizando Arduino.
Se aloja un servidor web donde conociendo la IP que el router le asigno al dispositivo accedes a la página web donde se configura las tareas a ejecutar.

# Configuracion Inicial
Paso 1 - Cuando arranca el ESP8266 los primeros 10 segundos indicados por el parpadeo de un led para presionar un pulsador para que ingrese en modo Access Point con la IP 192.168.4.1 (en el navegador web) donde configuramos la Red wifi que se debe conectar. Presionamos en guardar el dispositivo se reinicia y repite el proceso, Pasados los 10 segundos se conecta a la red wifi previamente configurada. 

Paso 2 - Buscamos en la red la ip que el router le asigno y accedemos a la página principal.

# Las Tareas

Las tareas consisten en: 
1. Nombre de la tarea.
2. Fecha y Hora de inicio y fin.
3. Si esta activa o desactivada.
4. Dentro de ella hay 4 secuencias que se pueden programar. Consisten en:
⋅⋅1. Color (Depende si el cartel tiene esa capacidad. En el caso de que no, no mostrar)
⋅⋅2. Tipo de secuencia.
⋅⋅3. Invertir sentido.
⋅⋅4. Numero de repeticiones antes de pasar a la siguiete secuencia.
⋅⋅5. Tiempo de cada segmento antes de pasar al siguiente.
  
#Mas info

Comunicacion entre el Servidor y Cliente mediante Web Socket usando JSON.
Interfaz estilo web [Material Desing Lite](https://getmdl.io/)
