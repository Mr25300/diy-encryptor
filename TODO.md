- Finish test framework (printing)
- Replace Vector and Matrix class methods with functions applied to byte arrays
- Add operator overloads for GF256 w/ uint8_t so that arithmetic and comparisons work
- Fix all compilation warnings
- Make PBKDF2, HMAC and SHA256 faster
- Refactor AES into general Rijndael cipher but create specific policies for AES
- Make github run tests

- Move implementations to separate source file wherever possible
- Make IO methods use vectors only (not arrays)
- Switch to C++20 to use spans (especially to make CBC mutative and PBKDF2/HMAC/SHA256 more efficient)

- Fix formatting for constructors/functions with many parameters (make neovim do it automatically)
- Improve GF256 printing in blocks and vectors by manipulating cout stream state (similar to std::cout << std::hex) instead of passing in GFFormat
- Organize classes and functions into namespaces better
- Clean up code style (use correct methods of initialization, etc.)
- Consider using expression templates
- Add way to measure non-linearity with Hamming distance for SubBox
- Add way to check if matrix is Maximum Distance Separable (MDS) --> Show that every submatrix is non-singular (non-zero determinant)
