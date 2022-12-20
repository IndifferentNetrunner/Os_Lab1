#include <iostream>
#include "Manager.hpp"
#include "MyProcess.hpp"
#include "MyNamedPipe.hpp"

void Compute(std::string const& func, int val) {
    HANDLE pipe = nullptr;

    auto result = (func == "F_process") ? os::lab1::compfuncs::trial_f<os::lab1::compfuncs::DOUBLE_MULT>(val): os::lab1::compfuncs::trial_g<os::lab1::compfuncs::DOUBLE_MULT>(val);
    auto pipeName = (func == "F_process") ? R"(\\.\pipe\F_pipe)" : R"(\\.\pipe\G_pipe)";

    pipe = MyNamedPipe::Connect(pipeName);
    MyNamedPipe::Write(static_cast<HANDLE *>(pipe), result);
    CloseHandle(pipe);
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        while (true) {
            Manager manager(argv[0]);
            manager.launch();
            std::cout << "\n";
        }
    }
    else {
        if (strcmp(argv[1], "F_process") == 0 || strcmp(argv[1], "G_process") == 0) {
            Compute(argv[1], std::stoi(argv[2]));
        }
        else {
            std::cerr << "Unexpected command line arguments.";
            return 1;
        }
    }
    return 0;
}