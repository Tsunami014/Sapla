# Sapla
A cool flashcard app!

## Features


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

