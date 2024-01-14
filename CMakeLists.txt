cmake_minimum_required (VERSION 3.8)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

project ("Test_exerceise")

add_executable (Test_exerceise "Test_exerceise.cpp")

target_link_libraries(Test_exerceise PRIVATE nlohmann_json::nlohmann_json)
