
target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deeptravel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Logger.cpp
Parser.cpp
ProgressIndicator.cpp

)

target_sources(deeptravel PRIVATE

Buffer.cpp
Chrono.cpp
Exception.cpp
IO.cpp
Logger.cpp
Parser.cpp
ProgressIndicator.cpp

)
