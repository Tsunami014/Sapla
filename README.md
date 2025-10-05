# Sapla
A cool flashcard app made in QT. Flashcards are arranged in a grid and you are put under a time limit.

## Features
- Flashcards in a grid!
- Adds new flashcards with a timer
- Get coins for doing flashcards right and lose them if you get it wrong
- Buy time extensions
- When the board gets full you lose!

## Demo
https://github.com/user-attachments/assets/9885b8d0-9aff-4a45-96d2-a8b8d44cca54

## Controls
- Click on a flashcard when you think you know what it is
    - It now shows you the other side
    - Press space if you got it wrong (& lose some coins)
    - Press enter if you got it right (& gain coins)
- Press `\`` to increase the timer by 5 seconds (if it doesn't work, that's because you don't have enough coins. See the top right.)
- If you lose press any key to start again.

## Editing the flashcards
To find where the flashcards are located, run the app through the terminal. There it will print the location of the flashcard file. Find that file and open it and edit it.

Lines are split in groups of 3; the first line is a 't', the second is the front of the card and the third is the back. You can add more following the same pattern. Empty lines or lines starting with '#' will be ignored (as long as the groups of 3 stay together)

E.g.
```
t
Hello
Goodbye
```
Is a flashcard where the front says 'Hello' and the back says 'Goodbye'.
```
t
Hello

Goodbye
```
Is NOT a flashcard and will give errors as there is a blank space in the wrong spot. A blank space before the 't' or after the 'Goodbye' will be ok.

## The program refuses to start!
Run the app in the terminal. If it's a problem with the flashcard configuration file, it will print out what the problem is.

## Getting it
### The easy way
Download the executable from the releases tab
### The hard way (build from source)
1. Ensure you have cmake (on any Linux just install `cmake`, and on Windows if you have chocolatey you can `choco install cmake`)
2. Ensure QT is installed (and the svg library too)
    - For Linux, find which package it is.
        - Ubuntu:
```
sudo apt install qt6-base-dev qt6-svg-dev
```
        - Arch:
```
sudo pacman -S qt6-base qt6-svg
```
    - For Windows I guess you can run this if you have chocolatey and if not that's your problem.
```
choco install qt6 qt6-svg
```
3. 
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel --config Release
```
3. Go find the executable in the build folder! (Usually at the top level)

## ChatGPT usage
Because I have barely coded in C++ or used QT, I required a lot of finding out how to do stuff. BUT, I wrote almost every line of code myself (you copy paste only what you need from Google because everything else is useless so too do I with chatGPT). I just used its judgement for generalised problems ('how do I do xyz' not '(re)write abc program to do xyz') (like using Google) and helped debugging by pointing out what the problems were (but I fixed the problems myself) as that saved me a lot of unecessary time (and also it means the codebase is much neater).

