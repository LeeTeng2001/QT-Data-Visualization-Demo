This project is done in intel MacOS 11.6, with Clion + Qt6 framework 

## How to build or run

- Make sure cmake and qt is installed in your machine if you want to build the project

  ```bash
  # brew is package manager in osx
  brew install qt  # qt 6, technically it should work with qt5 too
  brew install cmake
  ```
- open project in clion, it'll detect cmakelist and auto configure, build project in release mode and run

- If you're using window you can try to compile the program using visual studio since cmakelist.txt is provided

- Alternatively, you could build manually from the command line

  ```bash
  cd SemEndProjectQT
  mkdir build && cd build
  cmake ../ -DCMAKE_BUILD_TYPE=RELEASE && make
  ./SemEndProjectQT  # Run the binary
  ```

