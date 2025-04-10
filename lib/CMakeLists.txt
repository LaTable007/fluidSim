include(FetchContent)

# SFML
FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML"
    GIT_TAG master
)
set(SFML_BUILD_AUDIO FALSE) # Désactiver les modules inutiles
set(SFML_BUILD_NETWORK FALSE)
FetchContent_MakeAvailable(sfml)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

# ImGui-SFML
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY "https://github.com/SFML/imgui-sfml"
    GIT_TAG master
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)
FetchContent_MakeAvailable(imgui-sfml)
