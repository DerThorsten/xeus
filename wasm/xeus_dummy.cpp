/* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <memory>

#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <emscripten/bind.h>

#include "test_interpreter.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_emscripten.hpp"


using namespace emscripten;

template<class interpreter_t>
class ems_interpreter : public interpreter_t
{
public:
    ems_interpreter()
    : interpreter_t(){

        // wrap in an object
        std::function<const nl::json&()>
        parent_header([&](){
            return m_parent_header;
        });
        this->register_parent_header(parent_header);
    }

    void set_parent_header_str(const std::string & parent_header_str)
    {
        m_parent_header = nl::json::parse(parent_header_str);
    }
    std::string get_parent_header_str()const
    {
        return m_parent_header.dump();
    }
private:
    nl::json m_parent_header; 
};



EMSCRIPTEN_BINDINGS(my_module) {


    class_<zmq::message_t>("message_t")
        .constructor<>()
    ;

    register_vector<zmq::message_t>("message_vector_t");

    class_<zmq::multipart_t>("multipart_t")
        .constructor<>()
        .constructor<std::string>()
        .function("pushstr", &zmq::multipart_t::pushstr)
        .function("addstr", &zmq::multipart_t::addstr)
        .function("str", &zmq::multipart_t::str)
    ;

    class_<nl::json>("nl_json")
    ;


    using interpreter_base_type = xeus::xinterpreter;
    using interpreter_type =    test_kernel::test_interpreter;
    using ems_interpreter_type =    ems_interpreter<interpreter_type>;

    class_<ems_interpreter_type>("test_interpreter")
        .constructor<>()
        .function
        <
        std::string(
            ems_interpreter_type&,
            std::string ,
            bool ,
            bool ,
            std::string,
            bool 
        )>
        ("execute_request",[](
            ems_interpreter_type& self,
            const std::string& code,
            bool silent,
            bool store_history,
            std::string user_expressions_json_str,
            bool allow_stdin
        )->std::string{
            auto user_expressions = nl::json::parse(user_expressions_json_str);
            return self.execute_request(code, silent, store_history, user_expressions, allow_stdin).dump();
        })

        .function<void( ems_interpreter_type&,emscripten::val)>("register_publisher", [](
            ems_interpreter_type& self, emscripten::val val
        ){
            // wrap in an object
            std::function<void(const std::string&, nl::json, nl::json, xeus::buffer_sequence)>
            publisher([val](const std::string& a, nl::json b, nl::json c, xeus::buffer_sequence buffer_sequence){
                std::cout<<"size of buffer_sequence "<<buffer_sequence.size()<<"\n";
                val(a, b.dump(), c.dump(), buffer_sequence);
            });
            self.register_publisher(publisher);
        })
        .function("get_parent_header_str", &ems_interpreter_type::get_parent_header_str)
        .function("set_parent_header_str", &ems_interpreter_type::set_parent_header_str)

    ;

    // class_<interpreter_type, base<ems_interpreter_type>>("test_interpreter")
    //      .constructor<>()
    // ;

}