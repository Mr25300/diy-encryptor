#include <vector>
#include <cstdint>

class PRF {
public:
    virtual void compute() const = 0;
    virtual size_t outputSize() const = 0;
};

class HMAC : public PRF {
public:
    void compute() {
        return;
    }

    size_t outputSize() {
        return 0;
    }
};

class PBKDF2 {
private:
    const std::vector<uint8_t>& salt;
    int iterations;

public:
    PBKDF2(const PRF& prf, const std::vector<uint8_t>& salt, int iterations) : salt(salt), iterations(iterations) {}

    std::vector<uint8_t> pseudorandomFunction(const std::vector<uint8_t>& input) {
        return {};
    }

    std::vector<uint8_t> deriveKey(const std::vector<uint8_t>& password, size_t keyLength) {
        // if (keyLength > (2 ^ 32 - 1) * hLen) throw error (hLen = length of pseudorandom output)

        return {};
    }
};
