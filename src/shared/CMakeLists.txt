target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Options.cpp
Coord.cpp
Logger.cpp
ProgressIndicator.cpp

)

target_sources(deepflight PRIVATE

Options.cpp
Coord.cpp
Logger.cpp
ProgressIndicator.cpp

)
