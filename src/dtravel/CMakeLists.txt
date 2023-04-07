target_include_directories(deeptravel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deeptravel PRIVATE

FFmpeg.cpp
Recorder.cpp
Traveller.cpp

)
