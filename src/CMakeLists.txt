add_library(toy_lib lib.cpp)
target_include_directories(toy_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(toy_app main.cpp)
target_link_libraries(toy_app PRIVATE toy_lib)
