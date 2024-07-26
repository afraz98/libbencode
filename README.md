# libbencode

This library provides functions for encoding and decoding data using the B-encode format, commonly used in torrent files. It supports integers, strings, lists, and dictionaries as defined in the Bencode specification.

Features
- Encode and decode B-encoded data.
- Handle integers, strings, lists, and dictionaries.
- Read Bencoded data from files or strings.
- Print Bencoded data in a JSON-like format.

## Installation

Clone the repository and compile the source files:

```sh
git clone https://github.com/afraz98/libbencode.git
cd libbencode
mkdir build
cd build
cmake ..
make
```

## Usage

### Decoding B-encoded Data

You can decode B-encoded data from a file or a string.

#### Decoding a File

To decode from a file, do the following:

```c

#include "bencode.h"

int main() {
    bencode_value *val = bencode_decode_file("example.torrent");
    if (val) {
        print_bencode_value(val, 0);
        free_bencode_value(val);
    }
    return 0;
}
```

#### Decoding from a String

To decode from a string, do the following:

```c

#include "bencode.h"

int main() {
    const char *input = "d3:keyi42ee";
    bencode_value *val = bencode_decode_str(input);
    if (val) {
        print_bencode_value(val, 0);
        free_bencode_value(val);
    }
    return 0;
}
```

### Encoding Bencoded Data

You can encode data into B-encode format using the `bencode_encode` function.

```c

#include "bencode.h"

int main() {
    // Create a b-encode integer
    bencode_value int_val = { .type = B_ENCODED_INTEGER, .integer = 42 };

    char output[256];
    size_t offset = 0;
    bencode_encode(&int_val, output, &offset);

    printf("Encoded B-encode: %s\n", output);
    return 0;
}
```

## Running Tests

A set of tests is provided to validate the functionality of the library. Compile and run the test suite as follows:

```sh

mkdir build
cd build
cmake ..
./test_bencode
```

## API Reference


### Structures

#### bencode_value

Represents a B-encode value.

```c

typedef struct bencode_value_struct {
    bencode_type type;
    union {
        long long integer;
        struct {
            char* string;
            int strlen;
        } string;
        
        struct {
            struct bencode_value_struct** list_val;
            int llen;
        } list;
        
        struct {
            struct bencode_value_struct** keys;
            struct bencode_value_struct** values;
            int dlen;
        } dict;
    };
} bencode_value;
```

### Functions

#### bencode_value* bencode_decode_file(const char* filename)

Decodes B-encoded data from a file.

    filename: Path to the file containing B-encoded data.
    Returns a pointer to a bencode_value structure.

#### bencode_value* bencode_decode_str(const char* input_str)

Decodes B-encoded data from a string.

    input_str: The input string containing B-encoded data.
    Returns a pointer to a bencode_value structure.

#### void bencode_encode(bencode_value *value, char *output, size_t *offset)

Encodes a bencode_value structure into B-encode format.

    value: The bencode_value to encode.
    output: The output buffer where the encoded data will be written.
    offset: Pointer to the current offset in the output buffer.

#### void print_bencode_value(bencode_value *value, int indent)

Prints a bencode_value structure in a JSON-like format.

    value: The bencode_value to print.
    indent: The current indentation level for pretty printing.

#### void free_bencode_value(bencode_value* obj)

Frees the memory allocated for a bencode_value structure.

    obj: The bencode_value to free.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request with your improvements.

## Acknowledgments

This library implements by the [B-encode](https://en.wikipedia.org/wiki/Bencode) format used in torrent files. A description of this protocol is provided [here](https://wiki.theory.org/BitTorrentSpecification#Bencoding).