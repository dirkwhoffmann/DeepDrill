# Build instructions

Follow the steps below to compile DeepDrill on your machine. The application has been designed to run on macOS and common Linux distributions such as Ubuntu or Mint.

- **Step 1: Install third-party libraries**

    To compile the DeepDrill tool chain, the GNU Multiple Precision Arithmetic Library (GMP) and the Simple and Fast Multimedia Library (SFML) are required. For recording zoom videos, DeepFlight executes FFmpeg at runtime.

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

    Compiling the application requires CMake and a standard C++ tool chain to be installed:
    ```bash
    cd DeepDrill
    mkdir build
    cd build
    cmake ../src
    make -j
    ```

On success, three executables have been build: `deepdrill`, `deepmake`, and `deepzoom`.
