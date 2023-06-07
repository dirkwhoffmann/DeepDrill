target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepmake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepzoom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Application.cpp
AssetManager.cpp
Options.cpp
Coord.cpp
Logger.cpp
Palette.cpp
ProgressIndicator.cpp
DrillMap.cpp
ColorMap.cpp
Colorizer.cpp
Filter.cpp

)

target_sources(deepmake PRIVATE

Application.cpp
AssetManager.cpp
Options.cpp
Coord.cpp
Logger.cpp
ProgressIndicator.cpp

)

target_sources(deepzoom PRIVATE

Application.cpp
AssetManager.cpp
Options.cpp
Coord.cpp
Logger.cpp
Palette.cpp
ProgressIndicator.cpp
DrillMap.cpp
ColorMap.cpp
Colorizer.cpp
Filter.cpp

)
