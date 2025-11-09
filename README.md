# Sapla
A cool flashcard app made in QT where you grow a tree.

FAQ is at the bottom.

## Features
### Notes & cards
- Simple custom notes which generate flashcards
- Browse all your notes, edit them, delete them or add more
- Markdown support - and reveals text underneath on selection
- Regular or double sided cards
- Saves notes to a configuration file automatically
### Gameplay
- Different flashcard 'games' - currently, a regular game and a grid game
- Disable games
- Grow a tree with your answers
### App
- Logging with popups
- Custom UI svgs *hand drawn* by me
- Responsive layout
- Menu with help information for ease of use

## Usage instructions
If you need help on anything, look at the `help > this screen help` or `help > application help`

## Demo video


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
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```
4. Go find the executable in the build folder! (Usually at the top level)
##### Packaging for Linux
```bash
./makeAppIm
```
#### Windows
You're on your own
# FAQ
## Did it save my changes?
Yes. Every time you change even one letter it saves to the file.

## How do I find the logs/config directory?
To see the logs: `Help` > `Logs`. Config directory is the first log on the list.

## ChatGPT usage
Because I have barely coded in C++ or used QT, I required a lot of finding out how to do stuff. BUT, I wrote almost every line of code myself (which means the codebase is much neater than if I'd just chatGPT'd it all). I just used its judgement for *generalised* problems ('how do I do xyz' not '(re)write abc program to do xyz') (like using Google) and helped debugging by pointing out what the problems were (but I fixed the problems myself) as that saved me a lot of unecessary time.

