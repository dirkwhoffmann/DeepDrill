target_include_directories(deepzoom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(deepzoom PRIVATE

FFmpeg.cpp
NamedPipe.cpp
Recorder.cpp
Zoomer.cpp

)
