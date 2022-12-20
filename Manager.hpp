#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <stdexcept>
#include <future>
#include "Windows.h"
#include "lib/compfuncs.hpp"
#include "lib/trialfuncs.hpp"
#include "MyProcess.hpp"
#include "MyNamedPipe.hpp"

class Manager {
private:
    int val;
    double res;
    std::string cmd;

    void calculateResult(std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> F,std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> G) {
        if (holds_alternative<double>(F) && holds_alternative<double>(G)) {
            res = std::get<double>(F) * std::get<double>(G);
            std::cout << " | " << "Result: " << res << "\n";
        }
        else {
            std::cout << " | " << "Result: |fail|\n";
        }
    }

public:
    explicit Manager(std::string cmd) : cmd(std::move(cmd)) {}

    void launch() {
        std::mutex mx;

        std::cout << " >> Enter case_value(0 - 5): ";
        std::cin >> val;

        const char* FPipeName = R"(\\.\pipe\F_pipe)";
        const char* GPipeName = R"(\\.\pipe\G_pipe)";
        auto FPipe = MyNamedPipe::Create(FPipeName);
        auto GPipe = MyNamedPipe::Create(GPipeName);

        std::string FProcessName = cmd + " F_process " + std::to_string(val);
        std::string GProcessName = cmd + " G_process " + std::to_string(val);
        auto FProcess = MyProcess::Create(FProcessName);
        auto GProcess = MyProcess::Create(GProcessName);

        std::thread ESC([]() {
            using namespace std::chrono_literals;
            if (GetAsyncKeyState(VK_ESCAPE) == 0) {
                std::this_thread::sleep_for(10ms);
            } else {
                std::cerr << "ESC pressed.\n";
                throw std::runtime_error("");
            }
        });

        std::promise<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double>> promiseF;
        std::promise<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double>> promiseG;

        auto producer = std::thread([&promiseF, &promiseG, &FPipe, &GPipe] {
            promiseF.set_value(MyNamedPipe::Read('f', &FPipe));
            promiseG.set_value(MyNamedPipe::Read('g', &GPipe));
        });

        auto futureF = promiseF.get_future();
        auto futureG = promiseG.get_future();

        std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> FFunction;
        std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> GFunction;

        auto consumer = std::thread([&futureF, &futureG, &FFunction, &GFunction]{
            FFunction = futureF.get();
            GFunction = futureG.get();
        });

        producer.join();
        consumer.join();
        ESC.join();

        calculateResult(FFunction,GFunction);
    }
};


#endif //MANAGER_HPP
