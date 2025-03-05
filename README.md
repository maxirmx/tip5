[![build-and-test](https://github.com/maxirmx/tip5/actions/workflows/build-and-test.yml/badge.svg?branch=main)](https://github.com/maxirmx/tip5/actions/workflows/build-and-test.yml)
[![codecov](https://codecov.io/gh/maxirmx/tip5/graph/badge.svg?token=R7ie8bhkCG)](https://codecov.io/gh/maxirmx/tip5)

# tip5xx

Tip5 hash (https://eprint.iacr.org/2023/107) implementation in C++

## Requirements

### C++ Implementation
- CMake 3.15 or higher
- C++17 compliant compiler
- For testing: Internet connection (to fetch Google Test and CLI11)

### Rust Implementation
- Rust 1.85 or higher
- Internet connection (to fetch dependencies)

## Building

### C++ Implementation
```bash
cmake -B build
cmake --build build
```

To run tests:
```bash
cd build && ctest --output-on-failure
```

### Rust Implementation
```bash
cd samples/tip5-rust
cargo build
```

## Build Options

### CMake Options
- `BUILD_TESTING=ON/OFF`: Enable/disable building tests (default: ON)
- `BUILD_SAMPLES=ON/OFF`: Enable/disable building sample applications (default: ON)
- `ENABLE_COVERAGE=ON/OFF`: Enable code coverage reporting (default: OFF)
- `ENABLE_SANITIZER=ON/OFF`: Enable Address Sanitizer (default: OFF)

## Usage

### As a C++ Library

```cpp
#include <tip5xx/tip5xx.hpp>

tip5xx::Tip5 processor;
processor.process("your input");
auto result = processor.result();
```

### Sample Applications

Both C++ and Rust implementations provide similar command-line interfaces supporting pair and variable-length hashing modes.

#### C++ Sample

```bash
# Pair mode with different number formats
./build/samples/tip5-cpp/tip5xx_sample 0x1EADB75F 0xC7FEBAB9     # hexadecimal (0x prefix)
./build/samples/tip5-cpp/tip5xx_sample 16909060 84281096         # decimal
./build/samples/tip5-cpp/tip5xx_sample 0100402404 0502060710     # octal (0 prefix)

# Variable-length mode with mixed formats
./build/samples/tip5-cpp/tip5xx_sample -m varlen 0x1EADB75F 16909060 0502060710

# Show help and options
./build/samples/tip5-cpp/tip5xx_sample --help
```

#### Rust Sample

```bash
cd samples/tip5-rust

# Pair mode with different number formats
cargo run -- 0x1EADB75F 0xC7FEBAB9          # hexadecimal (0x prefix)
cargo run -- 16909060 84281096              # decimal
cargo run -- 0100402404 0502060710          # octal (0 prefix)

# Variable-length mode with mixed formats
cargo run -- -m varlen 0x1EADB75F 16909060 0502060710

# Show help and options
cargo run -- --help
```

Options for both implementations:
- `-m, --mode <mode>`: Hashing mode ('pair' or 'varlen')
  - `pair`: Takes exactly 2 numbers (default mode)
  - `varlen`: Takes 2 or more numbers
- Input values support multiple formats:
  - Hexadecimal: Numbers with 0x prefix (e.g., 0x1EADB75F)
  - Decimal: Plain numbers (e.g., 16909060)
  - Octal: Numbers with leading 0 (e.g., 0100402404)

Note: Hex format requires the 0x prefix and even number of digits.

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request
