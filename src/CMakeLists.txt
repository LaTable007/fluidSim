add_executable(SimulationDeFluide main.cpp Balle.cpp Box.cpp Start.cpp StartRandom.cpp)
target_link_libraries(SimulationDeFluide PRIVATE ImGui-SFML::ImGui-SFML)