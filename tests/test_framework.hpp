#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <list>
#include <cstddef>
#include <chrono>

using std::chrono::time_point;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace test {
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    struct TestResults {
        std::size_t total;
        std::size_t failed;
        milliseconds time;
    };

    class TestSuite {
        std::vector<TestCase> tests{};

    public:
        const std::string name;
 
        TestSuite(std::string_view name, std::vector<TestCase> tests = {}) : name(name), tests{tests} {}

        template <typename F>
        void addCase(std::string_view name, F&& func) {
            tests.push_back(TestCase{std::string(name), std::forward<F>(func)});
        }

        TestResults run() const;
    };

    class TestFramework {
        // vector cannot be used as reallocations would create dangling pointers
        std::list<TestSuite> suites{};

    public:
        TestSuite& createSuite(std::string_view name);

        bool run() const;
    };
}
