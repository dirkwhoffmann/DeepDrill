target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

SlowDriller.cpp
Driller.cpp
Approximator.cpp
ReferencePoint.cpp

)
