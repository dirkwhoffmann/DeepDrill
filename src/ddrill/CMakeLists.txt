target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

DrillOptions.cpp
Maker.cpp
Driller.cpp
Colorizer.cpp
Coefficients.cpp
Coord.cpp
DrillMap.cpp
Palette.cpp
ReferencePoint.cpp

)
