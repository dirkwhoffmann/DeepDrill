target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepmake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepzoom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

ExtendedDouble.cpp
ExtendedComplex.cpp
PrecisionComplex.cpp
StandardComplex.cpp

)

target_sources(deepmake PRIVATE

ExtendedDouble.cpp
ExtendedComplex.cpp
PrecisionComplex.cpp
StandardComplex.cpp

)

target_sources(deepzoom PRIVATE

ExtendedDouble.cpp
ExtendedComplex.cpp
PrecisionComplex.cpp
StandardComplex.cpp

)
