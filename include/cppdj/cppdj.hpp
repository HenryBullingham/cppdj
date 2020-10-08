// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#pragma once

/*

USAGE:

Before the dependency can be used, it has to be registered with the dependency manager (to do so, it requires a default constructor)


 cppdj::register_dep<TYPE_NAME>();


//Then, in whatever object you want the dependency to be injected in, you need to declare:

cppdj:dep<TYPE_NAME> m_dep, which will act like a pointer.

Example:

namespace superduper
{
    class Foo
    {
        void Bar()
    };
}

Registration:
~
    cppdj::register_dep<Foo>();
~

Usage:
~
cppdj::dep<superduper::Foo> m_foo;
m_foo->Bar();
~

Example 2: (Inheritance & Interfaces)

struct IBar
{
    virtual void Activate() = 0;
}

class Bar : public IBar
{
    virtual void Activate(){ ... }
}

Registration: (Register as the base class type)
~
    cppdj::register_dep<IBar, Bar>(); 
~

Usage: (Dependency on the base class type)
~
cppdj::dep<IBar> m_foo;
m_foo->Bar();
~



Notes & Assumptions:

-> Not registering an object of 2 different base types



*/


#if !defined(CPPDJ_INCLUDE_CPPDJ_HPP)
#define CPPDJ_INCLUDE_CPPDJ_HPP

#include <memory>
#include <string>
#include <unordered_map>

#ifdef CPPDJ_ENABLE_ASSERTS
#include <cassert>
#define cppdj_assert(x_) assert(x_)
#else
#define cppdj_assert(x_)
#endif

namespace cppdj
{
    /// <summary>
    /// Registers a single dependency with the injection system.
    /// Registers it so that dependencies of type 'DependencyBase' can be injected
    /// Returns true if the dependency could be registered
    /// </summary>
    /// <typeparam name="DependencyBase"></typeparam>
    /// <typeparam name="DependencySub"></typeparam>
    /// <returns></returns>
    template<typename DependencyBase, typename DependencySub = DependencyBase>
    inline bool register_dep();

    /// <summary>
    /// Unregisters a dependency of type Dependency
    /// Returns true if successful
    /// </summary>
    /// <typeparam name="Dependency"></typeparam>
    /// <returns></returns>
    template<typename Dependency>
    inline bool unregister_dep();

    /*
        A wrapper for injected dependencies
    */
    template<typename Dependency>
    class dep
    {
    private:
        std::shared_ptr<Dependency> m_instance;

    public:
        dep();

        Dependency* operator->();

        Dependency& operator*();
    };


    /*
    
    Details for implmentation
    
    */

    namespace detail
    {
        template<typename T>
        using observer = T;

        using type_id = int;

        //A global increasing counter
        inline type_id cppdj_get_unique_id();

        //Custom type info
        template<typename Dependency>
        struct cppdj_type_info
        {
        public:
            static const type_id get_type_id();
        };

        //Dependency Wrapper Base Class (type-erased)
        struct dependency_wrapper_base
        {
            virtual ~dependency_wrapper_base() = default;

            virtual type_id get_type_id() const = 0;

            virtual std::shared_ptr<void> get_ptr() const = 0;
        };

        //Dependency Wrapper Impl Class
        template<typename Dependency>
        struct dependency_wrapper_impl : public dependency_wrapper_base
        {
        private:
            std::shared_ptr<Dependency> m_dep;

        public:
            dependency_wrapper_impl(std::shared_ptr<Dependency> dep);

            virtual type_id get_type_id() const;

            virtual std::shared_ptr<void> get_ptr() const;
        };

        class dep_manager
        {
        private:
            std::unordered_map<type_id, std::unique_ptr<detail::dependency_wrapper_base>> m_dependency_map;

        public:
            static observer<dep_manager*> get();

            template<typename DependencyBase, typename DependencySub = DependencyBase>
            bool register_dep();

            template<typename Dependency>
            bool unregister_dep();

            template<typename Dependency>
            std::shared_ptr<Dependency> get_dep() const;
        };
    }

    /*
    
    Dependency wrappers
    
    */

    namespace detail
    {
        /*   Unique counter */

        inline type_id cppdj_get_unique_id()
        {
            static type_id i = 0;
            return ++i;
        }

        /* Type info */
        template<typename Dependency>
        inline const type_id cppdj_type_info<Dependency>::get_type_id()
        {
            //Static id for the type
            static type_id id = cppdj_get_unique_id();
            return id;
        }

        /*
            Dependency Wrapper
        */
        template<typename Dependency>
        dependency_wrapper_impl<Dependency>::dependency_wrapper_impl(std::shared_ptr<Dependency> dep) : m_dep(dep){}

        template<typename Dependency>
        type_id  dependency_wrapper_impl<Dependency>::get_type_id() const
        {
            return detail::cppdj_type_info<Dependency>::get_type_id();
        }

        template<typename Dependency>
        std::shared_ptr<void> dependency_wrapper_impl<Dependency>::get_ptr() const
        {
            return m_dep;
        }
 
        /*

    dependency_manager

*/

        observer<dep_manager*> dep_manager::get()
        {
            static std::unique_ptr<dep_manager> instance = std::make_unique< dep_manager>();
            return instance.get();
        }

        template<typename DependencyBase, typename DependencySub>
        inline bool dep_manager::register_dep()
        {
            //Register as the base class
            type_id id =  cppdj_type_info<DependencyBase>::get_type_id();
            auto it = m_dependency_map.find(id);

            if (it == m_dependency_map.end())
            {
                //The pointer will be a Base Type, and the Impl will be the sub type
                m_dependency_map[id] = std::make_unique<dependency_wrapper_impl<DependencyBase>>(std::make_shared<DependencySub>());
                return true;
            }

            return false;
        }

        template<typename Dependency>
        inline bool dep_manager::unregister_dep()
        {
            type_id id = cppdj_type_info<Dependency>::get_type_id();

            auto it = m_dependency_map.find(id);

            if (it != m_dependency_map.end())
            {
                m_dependency_map.erase(id);
                return true;
            }

            return false;
        }

        template<typename Dependency>
        inline std::shared_ptr<Dependency> dep_manager::get_dep() const
        {
            auto it = m_dependency_map.find(cppdj_type_info<Dependency>::get_type_id());

            if (it == m_dependency_map.end())
            {
                cppdj_assert(false && "Dependency hasn't been registered!");
                return nullptr;
            }

            return std::static_pointer_cast<Dependency>(it->second->get_ptr());
        }
    }

    template<typename DependencyBase, typename DependencySub>
    inline bool register_dep()
    {
        return detail::dep_manager::get()->register_dep<DependencyBase, DependencySub>();
    }

    template<typename Dependency>
    inline bool unregister_dep()
    {
        return detail::dep_manager::get()->unregister_dep<Dependency>();
    }

    /* 
    
        dependency
    
    */
    template<typename Dependency>
    dep<Dependency>::dep()
    {
        m_instance = detail::dep_manager::get()->get_dep<Dependency>();
    }

    template<typename Dependency>
    Dependency* dep<Dependency>::operator->()
    {
        return m_instance.get();
    }

    template<typename Dependency>
    Dependency& dep<Dependency>::operator*()
    {
        return *m_instance;
    }



}

#endif
