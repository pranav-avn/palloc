# palloc

`palloc` is a custom implementation of the `malloc` function in C. This project aims to provide a simple and efficient memory allocation library for educational purposes.

## Features

- Custom memory allocation and deallocation
- Simple and easy-to-understand codebase
- Suitable for learning and experimentation

## Getting Started

### Prerequisites

- GCC (GNU Compiler Collection)

### Building

To build the project, run the following command:

```sh
gcc -o palloc palloc.c
```

### Usage

Include the `palloc.h` header file in your C program and use `palloc` and `pfree` functions for memory allocation and deallocation.

```c
#include "palloc.h"

int main() {
    int *arr = (int *)palloc(10 * sizeof(int));
    // Use the allocated memory
    pfree(arr);
    return 0;
}
```

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- Inspired by various memory allocation tutorials and resources.
