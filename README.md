# Overview
This project is a command-line tool for symmetric encryption and decryption. Written from scratch in modern C++, the project focuses on safe and efficient memory handling and code modularity, allowing it to easily suit other block ciphers, modes of operation, hash functions and key derivation functions in the future.

**Note**: This project was created for educational purposes in order to explore low-level programming and cryptography.

# Highlights
- **Built From Scratch**: All core operations and primitives, including Rijndael, CBC, SHA256, HMAC and PBKDF2, are built from scratch according to the RFC specifications.
- **Simple to Use**: The CLI is easy to use, taking only a single input file, producing a single output file and having up to 4 arguments.
- **Performance**: All algorithms are in-place and heap allocations are avoided whenever possible, making the tool extremely efficient.
- **Rigorously Tested**: Each implementation has been tested for both validity and efficiency with a comprehensive test suite compiled from credible sources.

# Compilation & Testing
The build and test instructions for this project are fully described in the `Makefile`.

To test and compile the tool, run the following:
```bash
git clone https://github.com/Mr25300/diy-encryptor.git
cd diy-encryptor

make test # Run the test suite (shows detailed output)

make # Build the tool (output executable to ./build/diy-encryptor)
```

# Command-Line Interface & Usage

## Usage
`diy-encryptor <encrypt | decrypt> <input path> [options]`

## Flags
- `-o, --output <path>`: Directory to place the output file in, defaults to the parent directory of the input path.
- `-d, --delete`: Deletes the original input file after generating the output file.

## Examples
```bash
# Encrypt file, place into  and delete original
diy-encryptor encrypt /path/to/input/file -d

# Decrypt file and place in a different directory
diy-encryptor decrypt /path/to/input/file -o /path/to/output/dir
```
