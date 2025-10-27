# Sapla
A cool flashcard app made in QT where you grow a tree.

FAQ is at the bottom.

## Features
- Different flashcard 'games' - one built in, another is optional.
- Logging popups!
- Regular or double sided with optional prefixes
- Grow a tree with your answers
- Custom UI svgs *hand drawn* by me
- Responsive layout
- Browse all your flashcards, edit them, delete them or add more
- Menu with help information for ease of use
- Saves flashcards to a configuration file automatically

## Usage instructions
Game plugins go under `<config directory>/gamePlugs` (to find the config directory, look at `Help` > `Logs` in the app). Just drop the library there and the game will recognise it! (Unless you typed in the folder name wrong)

## Demo video
https://github.com/user-attachments/assets/ae13948b-8609-4b25-8d62-6a1915e5a382

Please note in the video I get insane odds and get the same text 3 times in a row, so I quit out of it and went back in which is why at one point the recording freezes. Also I'm speedrunning the cards bcos I just want to get through them to show you how it works

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
cmake --build build --parallel --config Release
```
4. Go find the executable in the build folder! (Usually at the top level)
##### Packaging for Linux
1. `pip install appimage-builder`
2. `appimage-builder --recipe conf.yaml`
3. The output .AppImage should be in this directory!
#### Windows
You're on your own
# FAQ
## Did it save my changes?
Yes. Every time you change even one letter it saves to the file.

## How do I find the logs/config directory?
To see the logs: `Help` > `Logs`. Config directory is the first log on the list.

## ChatGPT usage
Because I have barely coded in C++ or used QT, I required a lot of finding out how to do stuff. BUT, I wrote almost every line of code myself (which means the codebase is much neater than if I'd just chatGPT'd it all). I just used its judgement for *generalised* problems ('how do I do xyz' not '(re)write abc program to do xyz') (like using Google) and helped debugging by pointing out what the problems were (but I fixed the problems myself) as that saved me a lot of unecessary time.

