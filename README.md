# Introduction

Projet qui simule la mécanique des fluides de manières euleriennes.

## Installation

Clone le repo

```bash
git clone https://github.com/LaTable007/fluidSim.git
```

## Build

On build avec Ninja afin d'avoir une exécutable en Release mode et une en debug mode. 

```bash
cmake -s . -b build -g "ninja multi-config"
```

Build en version Release:

```bash
cmake --build build --config Release
```

Build en version Debug:

```bash
cmake --build build --config Debug
```

## Run

Release

```bash
./build/src/Release/SimulationDeFluide
```

Debug

```bash
./build/src/Debug/SimulationDeFluide
```
