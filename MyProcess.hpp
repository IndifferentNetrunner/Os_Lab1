#ifndef OS_LAB1_MYPROCESS_HPP
#define OS_LAB1_MYPROCESS_HPP

#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>

class MyProcess
{
private:
    HANDLE handler;
public:
    explicit MyProcess(MyProcess& process){
        handler = process.handler;
    }

    MyProcess(MyProcess&&) = default;

    ~MyProcess(){
        Terminate();
        CloseHandle(handler);
    }

    static PROCESS_INFORMATION Create(const std::string& process){
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        auto *cmd = const_cast<char *>(process.c_str());

        if (!CreateProcess(nullptr,
                           cmd,
                           nullptr,
                           nullptr,
                           false,
                           0,
                           nullptr,
                           nullptr,
                           &si,
                           &pi)
                ) {
            std::cerr << "Failed to create process for function computation.\n";
            throw std::runtime_error("");
        }
        return pi;
    }

    void Terminate() const{
        TerminateProcess(handler, 0);
    }
private:
};

#endif //OS_LAB1_MYPROCESS_HPP
