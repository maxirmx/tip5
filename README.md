[![build-and-test](https://github.com/maxirmx/tip5/actions/workflows/build-and-test.yml/badge.svg?branch=main)](https://github.com/maxirmx/tip5/actions/workflows/build-and-test.yml)
[![codecov](https://codecov.io/gh/maxirmx/tip5/graph/badge.svg?token=R7ie8bhkCG)](https://codecov.io/gh/maxirmx/tip5)

# tip5xx

Tip5 hash (https://eprint.iacr.org/2023/107) implementation in C++

## Всё просто :)

- BField - целочисленное поле вычетов по модулю
- BFieldElement - значение в BField
- Digest - точка в пятимерном линейном пространстве, координаты в котором задаются значениями из BField
- Tip5.hash_varlen - хэш функция Digest^N --> Digest ( то есть из N штук Digest вычисляется Digest, N > 1 )
- Tip5.hash_pair - хэш функция Digest x Digest --> Digest ( то есть из двух Digest вычисляется Digest), но при этом не частный случай Tip5.hash_varlen при N = 2

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
cd build/samples/tip5-cpp

# Pair mode with different number formats
./tip5 "(0x1,0x2,0x3,0x4,0x5)" "(0x6,0x7,0x8,0x9,0xa)"     # hexadecimal
./tip5 "(1,2,3,4,5)" "(6,7,8,9,10)"                         # decimal

# Variable-length mode with mixed formats
./tip5 -m varlen "(1,2,3,4,5)" "(6,7,8,9,10)" "(11,12,13,14,15)"

# Show help and options
./tip5 --help
```

Options for C++ implementation:
- `-m, --mode <mode>`: Hashing mode ('pair' or 'varlen')
  - `pair`: Takes exactly 2 digests (default mode)
  - `varlen`: Takes 2 or more digests
- Each digest must be in format (n1,n2,n3,n4,n5) where each number can be:
  - Hexadecimal: Numbers with 0x prefix (e.g., 0x1F)
  - Decimal: Plain numbers (e.g., 42)

#### Rust Sample

```bash
cd samples/tip5-rust

# Pair mode with different number formats
cargo run -- "(0x1,0x2,0x3,0x4,0x5)" "(0x6,0x7,0x8,0x9,0xa)"     # hexadecimal
cargo run -- "(1,2,3,4,5)" "(6,7,8,9,10)"                         # decimal

# Variable-length mode with mixed formats
cargo run -- -m varlen "(1,2,3,4,5)" "(6,7,8,9,10)" "(11,12,13,14,15)"

# Show help and options
cargo run -- --help
```

Options for Rust implementation:
- `-m, --mode <mode>`: Hashing mode ('pair' or 'varlen')
  - `pair`: Takes exactly 2 digests (default mode)
  - `varlen`: Takes 2 or more digests
- Each digest must be in format (n1,n2,n3,n4,n5) where each number can be:
  - Hexadecimal: Numbers with 0x prefix (e.g., 0x1F)
  - Decimal: Plain numbers (e.g., 42)

Note: Hex format requires the 0x prefix and even number of digits.

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request
