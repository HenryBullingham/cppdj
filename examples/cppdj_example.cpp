// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#define CPPDJ_ENABLE_ASSERTS

#include "cppdj/cppdj.hpp"

#include <iostream>

namespace cppdjtest
{
    class email_service
    {
    public:
        void send_email(std::string address, std::string message)
        {
            std::cout << "$SEND_EMAIL [ADDR=" << address << "] -> " << message << std::endl;
        }
    };
}

class my_service
{

private:
    //This is automatically populated at construction ('injected')
    cppdj::dep<cppdjtest::email_service> m_email_service;

public:
    void Operate()
    {
        m_email_service->send_email("Author", "CPPDI is a cool library!");
    }
};


class test_base
{
public:
    virtual void Foo() { std::cout << "test base" << std::endl; }
};

class test_impl : public test_base
{
    virtual void Foo() override { std::cout << "test impl!" << std::endl; }
};

int main()
{
    cppdj::register_dep<cppdjtest::email_service>();
    cppdj::register_dep<my_service>();
    cppdj::dep<my_service> myservice;

    myservice->Operate();

    //Example with subclassing
    cppdj::register_dep<test_base, test_impl>();
    cppdj::dep<test_base> m_test;
    m_test->Foo();

    return 0;
}