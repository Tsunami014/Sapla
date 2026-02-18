# Sapla
A cool flashcard app made in QT!

If you need any help, please use the `Help -> App help` or `Help -> This screen help` menu item. For FAQ and important things look at the app help.

## How it works
You define 'notes' which generate 'cards' based on the content.

The cards are created when features are added such as `---` for separating notes into single sided cards or `===` for double sided cards

There are example cards to help you

## Features
### Notes & cards
- Browse all your notes, edit them, delete them or add more
- Can add, copy, rename and delete decks of notes!
- Can make temporary decks that vanish on demand or app load
- Many different card features!
- Convenient help screen for card features you can look at while making new notes!
- Example cards to help
- A note preview which also reveals errors
- Markdown support - and reveals text underneath on selection
- Saves notes to a configuration file automatically as you type
- Tags & priority for ease of sorting
- Scheduling - cards revised based on their schedule time!
- Rate whether you know the card based on a scale!
### Gameplay
- Grow a tree with your answers
- Different short flashcard 'games' to test your knowledge in different ways
### App
- Neat logging with popups and a log view
- Custom UI svgs *hand drawn* by me that change colour based on the theme
- Responsive layout
- Help information for ease of use
- Menu with updating items
### Customisability
- Ease of plugin creation through the extensable code structure
- Import/disable/remove plugins
- Plugins can add new games, features, change the colour scheme or whatever!
#### Plugins
- Built in plugin which comes with the app, which adds
    - A simple flashcard game - just a regular flashcard
- Grid plugin, which adds
    - A flashcard game - multiple flashcards in a grid you have to work through
- Winter plugin, which adds
    - A new colour theme

## Usage instructions
If you need help on anything, look at the `help > this screen help` or `help > application help`

## Demo video
https://github.com/user-attachments/assets/25ea20ac-16c6-45a0-86a2-5bbb45f343fe

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

