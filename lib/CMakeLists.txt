add_library(requests requests.cpp requests.h)
add_library(weather_app weather_app.cpp weather_app.h)

target_link_libraries(requests cpr::cpr nlohmann_json::nlohmann_json)
target_link_libraries(weather_app cpr::cpr nlohmann_json::nlohmann_json requests)
#target_link_libraries(requests PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(weather_app nlohmann_json::nlohmann_json)
#target_link_libraries(weather_app requests)