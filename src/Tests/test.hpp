#pragma once
#include "QueueTest.hpp"
#include "CallbackTest.hpp"

namespace JNet {
    namespace test {
        TestResult test(std::ostream& output) {
            TestResult testResult;
            testResult.succeded = 0;
            testResult.tests = 0;
            TestResult tempTestResult;

            
            tempTestResult = queueTest(output);
            testResult.succeded += tempTestResult.succeded;
            testResult.tests += tempTestResult.tests;

            tempTestResult = callbackTest(output);
            testResult.succeded += tempTestResult.succeded;
            testResult.tests += tempTestResult.tests;






            if (testResult.tests == testResult.succeded) {
                output << fmt::format(fg(fmt::color::light_green), "All tests passed  {0}/{1}\n", testResult.succeded, testResult.tests);
                return testResult;
            }

            output << fmt::format(fg(fmt::color::red), "Not all tests passed  {0}/{1} passed\n", testResult.succeded, testResult.tests);
            return testResult;
        }
    }
}

