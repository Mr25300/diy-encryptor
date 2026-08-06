- Make PBKDF2, HMAC and SHA256 faster
- Refactor AES into general Rijndael cipher but create specific policies for AES
- Make github run tests

- Move implementations to separate source file wherever possible

- Fix formatting for constructors/functions with many parameters (make neovim do it automatically)
- Improve GF256 printing in blocks and vectors by manipulating cout stream state (similar to std::cout << std::hex) instead of passing in GFFormat
- Consider using expression templates
- Add way to measure non-linearity with Hamming distance for SubBox
- Add way to check if matrix is Maximum Distance Separable (MDS) --> Show that every submatrix is non-singular (non-zero determinant)
