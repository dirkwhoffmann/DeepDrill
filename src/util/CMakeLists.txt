target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepmake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepzoom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Parser.cpp
miniz.c

)

target_sources(deepmake PRIVATE

Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Parser.cpp

)

target_sources(deepzoom PRIVATE

Animated.cpp
Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Parser.cpp
miniz.c

)
