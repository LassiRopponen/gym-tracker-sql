add_library(sqlite3 sqlite3.c)

target_include_directories(sqlite3 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})