// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "catch.hpp"

#include "cppdj/cppdj.hpp"

class test_service
{
public:
    bool return_true()
    {
        return true;
    }
};

struct interface_base
{
    virtual int return_val() = 0;
};

class interface_derived_a : public interface_base
{
    virtual int return_val() override
    {
        return 5;
    }
};

class interface_derived_b : public interface_base
{
    virtual int return_val() override
    {
        return -7;
    }
};

class test_service_injected
{
private:
    cppdj::dep<test_service> m_service;

public:
    bool return_true_and_service()
    {
        return true && m_service->return_true();
    }
};

TEST_CASE("CPPDI", "[CPPDI]")
{
    SECTION("Registration")
    {
        REQUIRE(cppdj::register_dep<test_service>());
        REQUIRE_FALSE(cppdj::register_dep<test_service>()); // second registration should fail
    
        REQUIRE(cppdj::unregister_dep<test_service>());
    }

    SECTION("Registration Base class")
    {
        REQUIRE(cppdj::register_dep<interface_base, interface_derived_a>());
        REQUIRE_FALSE(cppdj::register_dep<interface_base, interface_derived_b>());

        REQUIRE(cppdj::unregister_dep<interface_base>());

    }

    SECTION("Register and get dependency")
    {
        REQUIRE(cppdj::register_dep<test_service>());

        cppdj::dep<test_service> service;

        REQUIRE(&(*service) != nullptr);
        REQUIRE(service->return_true());

        REQUIRE(cppdj::unregister_dep<test_service>());
    }

    SECTION("Register base a and get dependency")
    {
        REQUIRE(cppdj::register_dep<interface_base, interface_derived_a>());

        cppdj::dep<interface_base> service;

        REQUIRE(&(*service) != nullptr);
        REQUIRE(service->return_val() == 5);

        REQUIRE(cppdj::unregister_dep<interface_base>());
    }

    SECTION("Register base b and get dependency")
    {
        REQUIRE(cppdj::register_dep<interface_base, interface_derived_b>());

        cppdj::dep<interface_base> service;

        REQUIRE(&(*service) != nullptr);
        REQUIRE(service->return_val() == -7);

        REQUIRE(cppdj::unregister_dep<interface_base>());

    }

    SECTION("Register two and inject")
    {
        REQUIRE(cppdj::register_dep<test_service>());
        REQUIRE(cppdj::register_dep<test_service_injected>());

        cppdj::dep<test_service_injected> injected;
        REQUIRE(&(*injected) != nullptr);
        REQUIRE(injected->return_true_and_service());

        REQUIRE(cppdj::unregister_dep<test_service>());
        REQUIRE(cppdj::unregister_dep<test_service_injected>());
    }
}