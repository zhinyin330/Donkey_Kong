## Link of the [wiki](https://github.com/zhinyin330/Donkey_Kong/wiki)

## Link of the [video](https://www.youtube.com/shorts/RtC9i5QvrCU)

## Members:
- [Diyang Zhou](https://github.com/tomDyzu)
- [Xueheng Wang](https://github.com/xueheng519)
- [Zhiyi Zhou](https://github.com/zhinyin330)
- [Daniel Castillero](https://github.com/DaniCasti42)

## Description of the game:
The [Donkey Kong 1981](https://www.mariowiki.com/Donkey_Kong_(game)) a classic platform game where Mario must rescue the princess from Donkey Kong. Jump over barrels, climb ladders, collect stars, and use a hammer to destroy obstacles. Survive two challenging levels with increasing difficulty, lightning storms, fire enemies, and bombs. Beat the clock to reach the princess and advance to the next level. Includes a leaderboard featuring the top 5 players

## How to play
The main goal of the game is to navigate each stage, avoid enemies and obstacles, and reach the top where Pauline is being held. If Mario touches an enemy or falls from a height, the player loses a life, when you use up all your lifes, game over.

Controles:
|Controls|	Action |
| ------------- | ------------- |
| ← → / A D |	Move horizontally |
| ↑ ↓	/ W S | Go up/down stairs |
| ESPACIO | Jump |
| J	| Use a hammer (hit barrels or fire goblins) |
| K	| Activate Star Power (temporary immunity) |
| P	| Pause |
| F1	| Cheats |
| ESC	| Exit the game|

## List of features implemented
### Gameplay:
* Full Mario controls: horizontal movement, jumping, and climbing ladders
* Platform physics with precise collision detection
* Independent hitbox system for barrels (smaller hitbox)
* Jump over barrels (+100 points) with improved detection
* Life system (3 lives, Game Over when all are lost)
* Respawn after losing a life (score is preserved)
* Mario death animation with a 5-frame sequence
* Hammer power-up to destroy barrels (+100/+500 pts) with a 5-second cooldown
* Star power-up with temporary invincibility (10s, K key)
* Footstep, jump, ladder, and ambient sound effects

### Levels:
* Level 1: Classic stage with sloped platforms, rain, and oil barrels
* Level 2: Straight platforms, snow, and buttons that destroy platforms
* Score screen transition between levels
* 2-minute timer per level
* Clock with animated sprites
* Increasing difficulty with each level (barrels, bombs, fire enemies, lightning)
* Different music for each level

### Enemies:
* Donkey Kong:
  - Level 1: Throws barrels while holding a barrel, with looping animation
  - Level 2: Decorative cycle (Idle → Emote1 → Emote2)
  - Collision removes one life

* Barrels:
  - Roll, fall through triggers, available in normal and blue types (+500 pts)
  - Spawn frequency increases with level difficulty
  - Can be destroyed with the hammer

* Bombs:
  - Explode when approached (Level 2), damage active on frames 4–5
  - Spawn frequency increases with level difficulty
  - Explosion sound effect

* Fire Enemies:
  - Fire enemies appear in Level 2
  - Can be eliminated with the hammer (+200 points)
  - Spawn frequency increases with level difficulty
  - Spawn on random platforms
  - Sound effect when defeated
  - Dramatic DK fall after stepping on all buttons (Level 2)
  - Bombs and fire enemies disappear after DK falls

### Princess:
* Idle animation with frame switching (Idle1 ↔ Idle2)
* Random “HELP!” effect (30% probability)
* Position changes after game events

### Effects:
* Lightning storm effect (white screen flash) in both scenes
  - Frequency increases with level difficulty
  - Thunder sound effect
* Animated rain (Level 1)
* Animated snow (Level 2)
* Death overlay when time runs out
* Floating score texts (+100, +300, etc.)
* Death sound effect

### Scoring System:
* Collect stars and jump over barrels (+100 points)
* Hammer bonuses: normal barrel (+100), blue barrel (+500)
* Level 2 items (+300 each)
* Buttons (+100 each)
* Persistent High Score displayed on screen
* Top 5 leaderboard (player name, level, score)
* Sorted by level and score
* Saved in a scores.txt file

### Game Management:
* Game Over screen featuring:
  - Sad Mario image (Mario_sad.png)
  - Name entry system (maximum 10 characters)
* Pause menu (P key) with:
  - Resume game
  - Main menu (Leaderboard)
  - Game controls (image + text)
  - Freezes all sprites and animations
* Leaderboard options: Continue Game or New Game
* Scene transition system
* Timer freezes while the game is paused

### Debug Menu (Cheats):
* Stars always at maximum
* Hammer with no cooldown
* Continue game option

### Interface:
* Hearts displaying remaining lives
* Star and hammer icons with text
* Time clock with animated sprites
* High Score displayed on screen
* Level text shown during transitions
