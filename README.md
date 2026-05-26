## Link of the [wiki](https://github.com/zhinyin330/Donkey_Kong/wiki)

## Link of the [video](https://www.youtube.com/shorts/RtC9i5QvrCU)

## Members:
- [Diyang Zhou](https://github.com/tomDyzu)
- [Xueheng Wang](https://github.com/xueheng519)
- [Zhiyi Zhou](https://github.com/zhinyin330)
- [Daniel Castillero](https://github.com/DaniCasti42)

## Description of the game:
El [Donkey Kong 1981](https://www.mariowiki.com/Donkey_Kong_(game)) Un clásico juego de plataformas donde Mario debe rescatar a la princesa de Donkey Kong. Salta sobre barriles, sube escaleras, recoge estrellas y usa un martillo para destruir obstáculos. Sobrevive a dos niveles desafiantes con dificultad creciente, tormentas eléctricas, enemigos de fuego y bombas. Vence el tiempo para alcanzar a la princesa y avanzar de nivel. Incluye una tabla de clasificación con los 5 mejores jugadores.

## How to play
The main goal of the game is to navigate each stage, avoid enemies and obstacles, and reach the top where Pauline is being held. If Mario touches an enemy or falls from a height, the player loses a life, when you use up all your lifes, game over.

Controles:
|Tecla |	Acción |
| ------------- | ------------- |
| ← → / A D |	Moverse horizontalmente |
| ↑ ↓	/ W S | Subir/Bajar escaleras |
| ESPACIO | Saltar |
| J	| Usar martillo (golpear barriles o duendes de fuego) |
| K	| Activar poder de estrella (inmunidad temporal) |
| P	| Pausa |
| F1	| Cheats |
| ESC	| Salir del juego |

## List of features implemented
### Jugabilidad:
* Control completo de Mario: movimiento horizontal, saltos, escaleras
* Física de plataformas con colisiones precisas
* Sistema de hitbox independiente para barriles (más pequeño)
* Salto sobre barriles (+100 puntos) con detección mejorada
* Sistema de vidas (3 vidas, Game Over al perderlas)
* Respawn después de perder una vida (mantiene puntuación)
* Animación de muerte de Mario con secuencia de 5 frames
* Martillo para destruir barriles (+100/500 pts) con cooldown de 5s
* Poder de estrella con inmunidad temporal (10s, tecla K) 
* Sonidos de pasos, saltos, escaleras y ambiente

### Niveles:
* Nivel 1: Clásico con plataformas inclinadas, lluvia y bidones de aceite
* Nivel 2: Plataformas rectas, nieve, botones que destruyen plataformas
* Transición con pantalla de puntuación entre niveles
* Temporizador de 2 minutos por nivel
* Reloj con sprites animados 
* Dificultad creciente con el nivel (barriles, bombas, fuegos, rayos)
* Música diferente por nivel

### Enemigos:
* Donkey Kong:
  - Nivel 1: Lanza barriles con barril en manos, animación de ciclo
  - Nivel 2: Ciclo decorativo (Idle → Emote1 → Emote2)
  - Colisión quita una vida
* Barriles:
  - Ruedan, caen en triggers, tipo normal y azul (500 pts)
  - Frecuencia aumentable con nivel
  - Destruibles con martillo
* Bombas:
  - Explotan al acercarse (Nivel 2), daño en frames 4-5
  - Frecuencia aumentable con nivel
  - Efecto de sonido de explosión
* Fuegos:
  - Enemigos de fuego (Nivel 2)
  - Eliminables con martillo (+200 puntos)
  - Frecuencia aumentable con nivel
  - Se generan en plataformas aleatorias
  - Efecto de sonido al eliminar
  - Caída dramática de DK al pisar todos los botones (Nivel 2)
  - Las bombas y fuegos desaparecen tras la caída

### Princesa:
* Animación idle con cambio de frames (Idle1 ↔ Idle2)
* Efecto "HELP!" aleatorio (30% probabilidad)
* Cambio de posición tras eventos del juego

### Efectos:
* Tormenta de rayos (flash blanco) en ambas escenas
  - Frecuencia aumentable con nivel
  - Sonido de trueno
* Lluvia animada (Nivel 1)
* Nieve animada (Nivel 2)
* Overlay de muerte al acabarse el tiempo
* Textos flotantes de puntuación (+100, +300, etc.)
* Sonido de muerte

### Sistema de Puntuación:
* Recoger estrellas, saltar barriles (+100)
* Martillo: barril normal (+100), azul (+500)
* Items del Nivel 2 (+300 cada uno)
* Botones (+100 cada uno)
* High Score persistente mostrado en pantalla
* Leaderboard con top 5 (nombre, nivel, puntuación)
* Ordenado por nivel y puntuación
* Guardado en archivo scores.txt

### Gestión de Partida:
* Pantalla de Game Over con:
  - Mario triste (Mario_sad.png)
  - Entrada de nombre (máx 10 caracteres)
* Menú de pausa (tecla P) con:
  - Continuar partida
  - Menú principal (Leaderboard)
  - Controles del juego (imagen + texto)
  - Congela todos los sprites
* Leaderboard: Continuar partida o Nueva partida
* Sistema de transición entre escenas
* Tiempo se congela en pausa

### Debug Menu (Cheats):
* Estrellas siempre al máximo
* Martillo sin cooldown
* Continuar partida

### Interfaz:
* Corazones mostrando vidas restantes
* Iconos de estrellas y martillo con texto
* Reloj de tiempo con sprites 
* High Score en pantalla
* Texto de nivel en transiciones


 









