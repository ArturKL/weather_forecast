add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} requests)
target_link_libraries(${PROJECT_NAME} weather_app)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})