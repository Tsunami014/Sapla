# Sapla
A cool flashcard app made in QT. Flashcards are arranged in a grid and you are put under a time limit.

FAQ is at the bottom.

## Storyline
You are a wizard/witch/whatever who wants to grow a forest. But waiting 100 years for trees to grow is boring. Instead, you cast spells on the tree and once enough magic energy is absorbed in the tree it will grow! But you must be fast; otherwise the magic will slowly fade away.

(It's a metaphor for your mental growth remembering flashcards)

## Features
- Flashcards in a grid!
- Adds new flashcards with a timer
- Grow a tree with your answers
- Custom tree pictures hand drawn by me (in fact everything you see was drawn by me)
- Browse all your flashcards, edit them, delete them or add more
- Keybinds down the bottom of the screen for ease of use
- Saves flashcards to a configuration file

## Controls
Controls for things that are not obvious are down the bottom of the screen. Here are the controls for the main game (so you aren't reading under a time limit):
- Click on a flashcard when you think you know what it is
    - It now shows you the other side
    - Press space if you got it wrong
    - Press enter if you got it right

## Getting it
### The easy way
Download the executable from the releases tab
### The hard way (build from source)
#### Linux
1. Ensure you have cmake installed (on any Linux just install `cmake`)
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
3. 
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel --config Release
```
4. Go find the executable in the build folder! (Usually at the top level)
#### Windows
1. Get QT Creator (you can use `choco install qtcreator`)
2. Build it for release there (figure out how yourself, sorry it's not as straightforward as Linux)

# FAQ
## Did it save my changes?
Yes. Every time you change even one letter it saves to the file.

## The program refuses to start!
In the EXTREMELY unlikely case you've screwed something up with either the config file or have some weird permissionless state and the app refuses to run, run the app in the terminal and it may print the problem (if it's one of the edge cases I covered).

## ChatGPT usage
Because I have barely coded in C++ or used QT, I required a lot of finding out how to do stuff. BUT, I wrote almost every line of code myself (which means the codebase is much neater than if I'd just chatGPT'd it all). I just used its judgement for *generalised* problems ('how do I do xyz' not '(re)write abc program to do xyz') (like using Google) and helped debugging by pointing out what the problems were (but I fixed the problems myself) as that saved me a lot of unecessary time.

