# Build instructions

In this tutorial, you will learn how to download, compile, and install the DeepDrill toolchain. 

DeepDrill is a Unix-style C++ application and should be compatible with most Unix-style operating systems. It has been tested to run on macOS, Ubuntu, and Linux Mint.

- **Step 1: Install third-party libraries**

    To compile the DeepDrill toolchain, the GNU Multiple Precision Arithmetic Library (GMP) and the Simple and Fast Multimedia Library (SFML) are required. For recording zoom videos, FFmpeg needs to be installed on your machine. It is executed by DeepZoom at runtime.

  - Option 1: Install with Homebrew
 
    ```bash
    brew install gmp
    brew install libsfml
    brew install ffmpeg
    ```

  - Option 2: Install with APT

    ```bash
    sudo apt-get install libgmp3-dev
    sudo apt-get install libsfml-dev
    sudo apt-get install ffmpeg
    ```

- **Step 2: Clone the GitHub project**

    ```bash
    git clone https://github.com/dirkwhoffmann/DeepDrill.git
    ```

- **Step 3: Compile the source code**

    Compiling the application requires CMake and a standard C++ toolchain to be installed:
    ```bash
    cd DeepDrill
    mkdir build
    cd build
    cmake ../src
    make -j
    ```

The buid process creates three executables: `deepdrill`, `deepmake`, and `deepzoom`.
