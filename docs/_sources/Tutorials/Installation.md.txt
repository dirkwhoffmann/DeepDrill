# Build instructions

Follow the steps below to compile DeepDrill on your machine. The application has been designed to run on macOS and common Linux distributions such as Ubuntu or Mint.

- **Step 1: Install third-party libraries**

    DeepDrill utilizes the GNU Multiple Precision Arithmetic Library (GMP) and the tool raw2tiff from the libtiff package. 

  - Option 1: Install with Homebrew
 
    ```bash
    brew install gmp
    brew install libtiff
    ```

  - Option 2: Install with APT

    ```bash
    sudo apt-get install libgmp3-dev
    sudo apt-get install libtiff-tools
    ```

- **Step 2: Clone the GitHub project**

    ```bash
    git clone https://github.com/dirkwhoffmann/DeepDrill.git
    ```

- **Step 3: Compile the source code**

    Compiling the application requires a standard C++ tool chain to be installed:
    ```bash
    cd DeepDrill/src
    make
    cd ..
    ```

On success, a `bin`</code>` directory has been created which contains the `deepdrill`</code>` executable.
