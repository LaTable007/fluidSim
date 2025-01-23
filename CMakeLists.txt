
# Version minimale requise de CMake
cmake_minimum_required(VERSION 3.16)

# Nom du projet
project(SimulationDeFluide LANGUAGES CXX)

# Définir la version de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ajouter les fichiers source
add_executable(${PROJECT_NAME} src/main.cpp src/Start.cpp src/Balle.cpp src/Box.cpp src/StartRandom.cpp)


# Trouver la bibliothèque SFML
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

# Lier SFML à l'exécutable
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics)

# Inclure les répertoires pour les headers
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Générer les commandes de compilation (utile pour les LSP et IDE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
