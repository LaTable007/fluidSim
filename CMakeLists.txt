cmake_minimum_required(VERSION 3.15)

project(SimulationDeFluide
        LANGUAGES CXX
        VERSION 1.0
)

# Définit la version du standard C++ à utiliser
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Ajoute les répertoires contenant les fichiers sources et les headers
add_subdirectory(include)
add_subdirectory(src)
