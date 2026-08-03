#include "test_framework.hpp"

#include <iostream>

namespace test {
    namespace color {
        constexpr std::string_view reset{"\033[0m"};
        constexpr std::string_view bold{"\033[1m"};
        constexpr std::string_view green{"\033[32m"};
        constexpr std::string_view red{"\033[31m"};
        constexpr std::string_view cyan{"\033[36m"};
        constexpr std::string_view gray{"\033[90m"};
    }

    TestResults TestSuite::run() const {
        std::size_t failed{};
        milliseconds msTotal{};

        for (const TestCase& test : tests) {
            std::cout << "  [ RUNNING ] " << color::cyan << test.name << color::reset << '\n';

            try {
                time_point t1{high_resolution_clock::now()};

                test.func();

                time_point t2{high_resolution_clock::now()};
                milliseconds ms{duration_cast<milliseconds>(t2 - t1)};

                msTotal += ms;

                std::cout << color::green << "  [ SUCCESS ] " << color::reset << ms.count() << "ms\n";

            } catch (const std::exception& e) {
                ++failed;

                std::cout << color::red << "  [ FAILURE ] " << color::gray << e.what() << color::reset << '\n';
            }
        }

        return {tests.size(), failed, msTotal};
    }

    TestSuite& TestFramework::createSuite(std::string_view name) {
        return suites.emplace_back(TestSuite{name});
    }

    bool TestFramework::run() const {
        const std::size_t maxSigns{20};
        std::size_t i{1};

        std::size_t total{};
        std::size_t failed{};
        milliseconds msTotal{};

        std::cout << "========== " << color::bold << "STARTING" << color::reset << " ==========\n";

        for (const TestSuite& suite : suites) {
            std::string barSigns(maxSigns * i / (suites.size() + 1), '=');

            std::cout << "=========> " << color::cyan << color::bold << suite.name << color::reset << '\n';

            TestResults results{suite.run()};

            total += results.total;
            failed += results.failed;
            msTotal += results.time;

            std::cout << "---------> ";
            if (results.failed == 0) std::cout << color::green << color::bold << "PASSED";
            else std::cout << color::red << color::bold << "FAILED (" << results.failed << ')';
            std::cout << color::reset << '\n';

            ++i;
        }

        std::cout << "========== " << color::bold << "FINISHED" << color::reset << " ==========\n"
            << "  Tests:   " << total << '\n'
            << "  Passed:  " << (total - failed) << '\n'
            << "  Failed:  " << failed << '\n'
            << "==============================\n";

        return failed == 0;
    }
}
