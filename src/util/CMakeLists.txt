
target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Parser.cpp

)

target_sources(deepflight PRIVATE

Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Parser.cpp

)
