target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deeptravel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepdrill PRIVATE

ExtendedDouble.cpp
ExtendedComplex.cpp
PrecisionComplex.cpp
StandardComplex.cpp

)

target_sources(deeptravel PRIVATE

ExtendedDouble.cpp
ExtendedComplex.cpp
PrecisionComplex.cpp
StandardComplex.cpp

)
