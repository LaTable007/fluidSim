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


# Ajoute les répertoires contenant les headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Ajoute les fichiers sources
add_subdirectory(lib)
add_subdirectory(src)


set_target_properties(SimulationDeFluide PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release
)
