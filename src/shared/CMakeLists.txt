target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

AssetManager.cpp
Options.cpp
Coord.cpp
Logger.cpp
Palette.cpp
ProgressIndicator.cpp

)

target_sources(deepflight PRIVATE

AssetManager.cpp
Options.cpp
Coord.cpp
Logger.cpp
Palette.cpp
ProgressIndicator.cpp

)
