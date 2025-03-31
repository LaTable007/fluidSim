include(FetchContent)

# SFML
FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML"
    GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
)
set(SFML_BUILD_AUDIO FALSE) # Désactiver les modules inutiles
set(SFML_BUILD_NETWORK FALSE)
FetchContent_MakeAvailable(sfml)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG 35b1148efb839381b84de9290d9caf0b66ad7d03
)
FetchContent_MakeAvailable(imgui)

# ImGui-SFML
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY "https://github.com/SFML/imgui-sfml"
    GIT_TAG 82dc2033e51b8323857c3ae1cf1f458b3a933c35
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)
FetchContent_MakeAvailable(imgui-sfml)
