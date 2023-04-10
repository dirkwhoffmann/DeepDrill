target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepflight PRIVATE

FFmpeg.cpp
NamedPipe.cpp
Recorder.cpp
Traveller.cpp

)
