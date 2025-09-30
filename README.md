# Sapla
A cool flashcard app!

## Features


## Getting it
### The easy way
Download the executable from the releases tab
### The hard way (build from source)
1. Ensure QT is installed
    - For Linux, find which package it is. Ubuntu is `qt6-base-dev` and arch is `qt6-base`
    - For Windows I guess you can `choco install qt6`
2. 
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel --config Release
```
3. Go find the executable in the build folder! (Usually at the top level)

