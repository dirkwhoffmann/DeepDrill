
target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Chrono.cpp
Parser.cpp
Exception.cpp
IO.cpp
Logger.cpp
ProgressIndicator.cpp

)
