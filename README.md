[![build-and-test](https://github.com/maxirmx/tip5/actions/workflows/build-and-test.yml/badge.svg?branch=main)](https://github.com/maxirmx/tip5/actions/workflows/build-and-test.yml)

# tip5xx

Tip5 implementation in C++

## Requirements

- CMake 3.15 or higher
- C++17 compliant compiler
- For testing: Internet connection (to fetch Google Test)

## Building

```bash
cmake -B build
cmake --build build
```

To run tests:
```bash
cd build && ctest --output-on-failure
```

## Build Options

- `BUILD_TESTING=ON/OFF`: Enable/disable building tests (default: ON)
- `BUILD_SAMPLES=ON/OFF`: Enable/disable building sample applications (default: ON)

## Usage

### As a Library

```cpp
#include <tip5xx/tip5xx.hpp>

tip5xx::Tip5 processor;
processor.process("your input");
auto result = processor.result();
```

### Sample Application

```bash
./build/samples/tip5xx_sample "your input"
```

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request
