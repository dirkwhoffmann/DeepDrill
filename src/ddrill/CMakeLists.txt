target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deeptravel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Options.cpp
Maker.cpp
Driller.cpp
Colorizer.cpp
Coefficients.cpp
Coord.cpp
DrillMap.cpp
Palette.cpp
ReferencePoint.cpp

)

target_sources(deeptravel PRIVATE

Options.cpp
Coord.cpp
Palette.cpp
ReferencePoint.cpp

)
