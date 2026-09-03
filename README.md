# Overview
This project is a command-line tool for symmetric encryption and decryption. Written from scratch in modern C++, the project focuses on safe and efficient memory handling and code modularity, allowing it to easily suit other block ciphers, modes of operation, hash functions and key derivation functions in the future.

**Note**: This project was created for educational purposes in order to explore low-level programming and cryptography.

# Design
The tool uses 4 main components during the symmetric encryption and decryption process:
- **Key Derivation Function (PBKDF2)**: Transforms the user-generated password and a random salt into a strong encryption key.
- **Block Cipher (AES)**: Encrypts and decrypts a fixed-size block of data using the derived key.
- **Mode of Operation (CBC)**: Chains cipher blocks together using an initialization vector (IV) so that identical plaintext blocks produce distinct ciphertexts. 
- **Padding Scheme (PKCS7)**: Appends padding bytes to the input data so that it aligns with the block size.

For portability, the IV and KDF salt are prepended to the ciphertext in a single file.

# Highlights
- **Built From Scratch**: All core operations and primitives, including Rijndael, CBC, SHA256, HMAC, and PBKDF2 are built from scratch according to their published specifications.
- **Simple to Use**: The CLI has only 2 positional arguments and 2 possible flags, making it extremely easy to learn and use.
- **Performant**: All algorithms are in-place and heap allocations are avoided wherever possible, making the tool extremely efficient.
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
# Encrypt a plaintext file, output the ciphertext file to the same directory, and delete the original file
diy-encryptor encrypt /path/to/input/file -d

# Decrypt a ciphertext file, output the plaintext file into a specific directory, and leave the original file
diy-encryptor decrypt /path/to/input/file -o /path/to/output/dir
```
