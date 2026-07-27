#include "test_framework.hpp"

#include <iostream>

namespace test {
    TestResults TestSuite::run() const {
        std::size_t failed{};
        milliseconds msTotal{};

        for (const TestCase& test : tests) {
            std::cout << "[ RUNNING ] " << test.name << '\n';

            try {
                time_point t1{high_resolution_clock::now()};

                test.func();

                time_point t2{high_resolution_clock::now()};
                milliseconds ms{duration_cast<milliseconds>(t2 - t1)};

                msTotal += ms;

                std::cout << "[ SUCCESS ] " << ms.count() << "ms\n";

            } catch (const std::exception& e) {
                ++failed;

                std::cout << "[ FAILURE ] " << e.what() << '\n';
            }
        }

        return {tests.size(), failed, msTotal};
    }

    TestSuite& TestFramework::createSuite(std::string_view name) {
        return suites.emplace_back(TestSuite{name});
    }

    bool TestFramework::run() const {
        std::size_t total{};
        std::size_t failed{};
        milliseconds msTotal{};

        for (const TestSuite& suite : suites) {
            std::cout << "Starting test suite \"" << suite.name << "\"\n";

            TestResults results{suite.run()};

            total += results.total;
            failed += results.failed;
            msTotal += results.time;

            std::cout << "Test suite \"" << suite.name << "\" " <<
                (failed == 0 ? "passed" : "failed") << '\n';
        }

        std::cout << "Ran " << total << " tests\n" <<
            (total - failed) << " passed\n" <<
            failed << " failed\n";

        return failed == 0;
    }
}
