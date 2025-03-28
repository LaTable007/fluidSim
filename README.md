# Introduction

Projet qui simule la mécanique des fluides de manières euleriennes.

## Installation

git clone le repo

## Build

Build utilise Ninja

```bash
cmake -S . -B build -G "Ninja Multi-Config"
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
