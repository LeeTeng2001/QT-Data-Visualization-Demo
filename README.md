This project is done in intel Mac 11.6, with Clion + Qt6 framework

## Features
- SQLite as data backend, 1 million entries only need roughly 1min to insert on my machine
- cached data for future usage
- Interactive realtime user movement simulation
- Multi thread to load data and do calculation

## Demo
- Top 10 POI from a set of user or top 10 users from a set of POI in a time frame
![Demo1](readme-sources/demo%20-%201.gif)
- Total POI over time, graphically tunable geographic and time bound
![Demo2](readme-sources/demo%20-%202.gif)
- Compare two cluster of user/POI set overtime
![Demo3](readme-sources/demo%20-%203.gif)
- Select a user to show its trajectory, you can use the slider to simulate its movement. Latitude, longitude and time will update in real-time
![Demo4](readme-sources/demo%20-%204.gif)
- Predict total POI at a date from a set of POI data, internally it's using cubic interpolation to do the prediction
![Demo5](readme-sources/demo%20-%205.gif)

## How to build or run

- Make sure both cmake and qt is installed in your machine if you want to build the project

  ```bash
  # brew is a package manager in MacOS
  brew install qt  # qt 6, technically it should also work with qt5
  brew install cmake
  ```
- open this project in clion, it'll detect cmakelist and auto configure, build this project in release mode and run the compiled binary

- If you're using window you can try to compile the program using visual studio since cmakelist.txt is provided

- Alternatively, you could manually build the project from the command line

  ```bash
  cd <this-project-folder>
  mkdir build && cd build
  cmake ../ -DCMAKE_BUILD_TYPE=RELEASE && make
  ./SemEndProjectQT  # Run the binary
  ```

