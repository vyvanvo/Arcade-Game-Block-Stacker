# Arcade Game: Block Stacker
Demo Links

Final Project Demo: https://youtu.be/VT4SVJy1_TI 
- Explained the complexities
- Played the game

Complexity 1: https://youtu.be/0tT-UEeZVoo 
- LED Matrix
- Two shift registers

Complexity 2: https://youtu.be/0Ym5bxLDB28 
- Microphone 
- Digital input

Complexity 3: https://youtu.be/VT4SVJy1_TI 
- USART
- Connection between two atmega 1284

## Project Overview
My final project is an arcade game called “Block Stackers.” The goal of this game is to stack the blocks until you reach the top or finish line. The game starts off with three blocks that will move side to side (left and right) at the top line. When the player hits the button, the block will drop. After the block drops, a new block will appear at the top line and move side to side. The player will have to hit the button again to drop the block to stack on top of the previous block. In this case, instead of using a button to drop the blocks, the player must yell into the microphone loud enough for the microphone to register their voice. If the block is misplaced, the piece of the block that is not on top of the bottom block will disappear. As a result, the player can only continue off from the stacks of blocks that already exist. If the player fails to stack the blocks to reach the top/finish line, the player loses. If the player stacks the block to the top/finish line, then they win. As more blocks are being stacked, the number of blocks on each row decreases. In addition to that, the speed of the block(s) moving side to side increases as the tower gets higher.
