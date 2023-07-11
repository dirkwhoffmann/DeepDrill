target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

Approximator.cpp
Driller.cpp
MapAnalyzer.cpp
ReferencePoint.cpp
SlowDriller.cpp

)
