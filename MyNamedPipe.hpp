#ifndef OS_LAB1_MYNAMEDPIPE_HPP
#define OS_LAB1_MYNAMEDPIPE_HPP

#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>

class MyNamedPipe
{
private:
    HANDLE handler = INVALID_HANDLE_VALUE;
public:
    explicit MyNamedPipe(MyNamedPipe& pipe){
        handler = pipe.handler;
    }

    MyNamedPipe(MyNamedPipe&& other) = default;

    ~MyNamedPipe(){
        CloseHandle(handler);
    }

    static void Write(HANDLE* pipe, std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> data){
        auto resultVoid = static_cast<void*>(&data);
        bool flag = WriteFile(
                pipe, // handle to our outbound pipe
                resultVoid, // data to send
                sizeof(data), // size of data to send (bytes)
                nullptr, // actual amount of data sent
                nullptr // not using overlapped IO
        );
        if (!flag) {
            std::cerr << "Failed to send data of function.\n";
            throw std::runtime_error("");
        }
    }

    static std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> Read(char func, HANDLE* pipe){
        ConnectNamedPipe(*pipe, nullptr);

        std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double> data;
        auto* buffer = static_cast<void*>(&data);

        bool flag = ReadFile(
                *pipe,
                buffer, // the data from the pipe will be put here
                sizeof(data), // number of bytes allocated
                nullptr, // number of bytes actually read
                nullptr // not using overlapped IO
        );
        if (!flag) {
            std::cerr << "Failed to read data from named pipe.\n";
            throw std::runtime_error("");
        }

        auto const* funcRes = static_cast<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, double>*>(buffer);

        std::cout << " | " << func << "(x): " << *funcRes << "\n";

        return *funcRes;
    }

    static HANDLE Create(const char* pipe_name){
        auto pipe = CreateNamedPipe(pipe_name,
                                    PIPE_ACCESS_DUPLEX ,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                    PIPE_UNLIMITED_INSTANCES,
                                    0,
                                    0,
                                    0,
                                    nullptr
        );
        if (pipe == nullptr || pipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create pipe instance.\n";
            throw std::runtime_error("");
        }

        return pipe;
    }

    static HANDLE Connect(const char* name){
        auto pipe = CreateFile(
                name,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
        );
        if (pipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to connect to pipe" << ".\n";
            throw std::runtime_error("");
        }
        return pipe;
    }
};

#endif //OS_LAB1_MYNAMEDPIPE_HPP
