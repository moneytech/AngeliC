cmake_minimum_required(VERSION 3.2)
project(AngeliC)

include_directories(include)

set(SOURCE_FILES main.c)
add_executable(AngeliC ${SOURCE_FILES} main.c include/angelic.h include/router.h include/buffer.h buffer.c router.c include/http_server.h http_server.c include/http.h http.c include/json.h)