# Arcade Game: Block Stacker
**Demo Links**

**Final Project Demo:** https://youtu.be/VT4SVJy1_TI 
- Explained the complexities
- Played the game

**Complexity 1:** https://youtu.be/0tT-UEeZVoo 
- LED Matrix
- Two shift registers

**Complexity 2:** https://youtu.be/0Ym5bxLDB28 
- Microphone 
- Digital input

**Complexity 3:** https://youtu.be/VT4SVJy1_TI 
- USART
- Connection between two atmega 1284

## Project Overview
My final project is an arcade game called “Block Stackers.” The goal of this game is to stack the blocks until you reach the top or finish line. The game starts off with three blocks that will move side to side (left and right) at the top line. When the player hits the button, the block will drop. After the block drops, a new block will appear at the top line and move side to side. The player will have to hit the button again to drop the block to stack on top of the previous block. In this case, instead of using a button to drop the blocks, the player must yell into the microphone loud enough for the microphone to register their voice. If the block is misplaced, the piece of the block that is not on top of the bottom block will disappear. As a result, the player can only continue off from the stacks of blocks that already exist. If the player fails to stack the blocks to reach the top/finish line, the player loses. If the player stacks the block to the top/finish line, then they win. As more blocks are being stacked, the number of blocks on each row decreases. In addition to that, the speed of the block(s) moving side to side increases as the tower gets higher.

## User Guide

**Rules:**
The player must stack the moving blocks until the blocks reach the top/finish line. The player should try to stack the blocks perfectly on top of each other, otherwise the game will be more challenging. If the player misplaces the block, the player can only stack the given block on top of the already existing block(s).

**Controls:**
The player can use either a microphone or button to play the game.

Microphone: To drop the block, the player must yell into the microphone loudly so that the system can register their voice.

Button: To drop the block, the player must press on the button. The player needs to release the button and press the button again if they wish to drop another block.

Reset Button:
Once the player wins or loses the game, the player can press the reset button to restart the game and play again. The reset button will only work when the end game display is showing on the LCD Screen. The end game display includes “Game Over!” and “You Win!” The reset button will not work if the display is showing “Game In Progress”.

**Special Considerations:**
Due to the lights on the LED Matrix flickering, the user must be able to handle this kind of display when playing the game. In addition to that, due to the microphone’s low sensitivity the player must have a loud voice for the microphone to register their voice input to drop the block. A person with a soft voice or does not yell loud enough will have a hard time playing this game as their voice might not be registered as an input.

## Component Visualization
![atmega 1284](https://dojft4652t1in.cloudfront.net/wp-content/uploads/IC-ATMEGA1284-PU-3.jpg)

**Sender microcontroller:**
    
    Inputs:
    - Microphone / Big Sound Sensor (PB6)
    - Reset Button (PB7)
    - USART Communication RXD0 (PD0)

    Outputs:
    - LED Matrix → Column Shift Register (PC3…PC0)
    - LED Matrix → Row Shift Register (PA3…PA0)
    - USART Communication TXD0 (PD1)

**Receiver microcontroller:**
    
    Inputs:
    - USART Communication RXD0 (PD0)

    Outputs:
    - LCD Screen (PORTC, PD6, PD7)
    - USART Communication TXD0 (PD1)

## Technologies Learned/Used
**LED Matrix and Shift Registers:**

The first complexity is the LED matrix, which is used to display the game. The display will show the blocks that have already been stacked by the player. In addition to that, the display also shows the blocks moving side to side and the blocks dropping once the player hits the button or yells into the microphone. 

The display shows three different cases:
Display with moving blocks at the top row and a tower below it
Display with perfectly stacked blocks
Display with misplaced blocks (where extra block(s) hanging on the side are truncated)

In addition to that, there is also a victory display at the end of the game if the player successfully stacks all of the blocks and wins the game. The LED matrix display will rain blocks down and then blink.

There are two shift registers that controls the LEDs on the LED Matrix. One shift register is used for the columns and another shift register is used for the rows of the LED Matrix. The shift registers is beneficial because it reduces the number of pins on the microcontroller that needs to be used.

**Microphone (Big Sound Sensor):**

The second complexity is the big sound sensor, which senses external sound. With digital input, when the player makes a loud noise, the system will register the sound as a digital input (input = 1) and then drop the block. Otherwise, if the sound is not loud enough for the system to register the sound (input = 0), then the blocks will continue to move side to side until the system is able to register a loud sound from the player to drop the block. 

**USART:**

The third complexity is using USART communication between two Atmega 1284 microcontrollers. The first microcontoller is the sender, which sends serial data to the second microcontroller. Based on the serial data received by the second microcontroller, which is a receiver, the second microcontroller executes certain actions.

In this case, the sender microcontroller sends information of the status of the game to the second microcontroller. This includes game in progress, win display, and game over display. Based the information sent, which indicates the status of the game, the second microcontroller displays the game status on the LCD screen.
