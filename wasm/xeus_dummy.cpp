/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
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


template<class smart_ptr>
struct leak_it
{
    leak_it(smart_ptr && ptr)
    : m_ptr(std::move(ptr))
    {

    }
    smart_ptr m_ptr;
};

// int main(int argc, char* argv[])
// {
//     // std::cout<<"Hello world\n";
//     // // std::string file_name = (argc == 1) ? "connection.json" : argv[2];
//     // // xeus::xconfiguration config = xeus::load_configuration(file_name);


//     xeus::xconfiguration config;
//     // // config.m_transport = doc["transport"].get<std::string>();
//     // // config.m_ip = doc["ip"].get<std::string>();
//     // // config.m_control_port = std::to_string(doc["control_port"].get<int>());
//     // // config.m_shell_port = std::to_string(doc["shell_port"].get<int>());
//     // // config.m_stdin_port = std::to_string(doc["stdin_port"].get<int>());
//     // // config.m_iopub_port = std::to_string(doc["iopub_port"].get<int>());
//     // // config.m_hb_port = std::to_string(doc["hb_port"].get<int>());
//     // // config.m_signature_scheme = doc.value("signature_scheme", "");



//     using history_manager_ptr = std::unique_ptr<xeus::xhistory_manager>;
//     history_manager_ptr hist = xeus::make_in_memory_history_manager();
    
//     using interpreter_ptr = std::unique_ptr<test_kernel::test_interpreter>;
//     auto interpreter = interpreter_ptr(new test_kernel::test_interpreter());
    
//     std::cout<<"done\n";
//     xeus::xkernel kernel(config,
//                          xeus::get_user_name(),
//                          std::move(interpreter),
//                          std::move(hist),
//                          nullptr,
//                          xeus::make_xserver_emscripten);
//     std::cout << "starting kernel" << std::endl;
//     kernel.start();
//     std::cout<<"done 2\n";

//     // std::cout << "countdown:\n";
//     // for (int i=2; i>0; --i) {
//     //     std::cout << i << std::endl;
//     //     std::this_thread::sleep_for (std::chrono::seconds(1));
//     // }
//     // std::cout << "Lift off!\n";
//     // auto leak = new leak_it<interpreter_ptr>(std::move(interpreter));
//     // std::cout << "Lift done!\n";
//     return 0;
// }


xeus::xkernel * make_xkernel()
{
    // std::cout<<"Hello world\n";
    // // std::string file_name = (argc == 1) ? "connection.json" : argv[2];
    // // xeus::xconfiguration config = xeus::load_configuration(file_name);


    xeus::xconfiguration config;
    // // config.m_transport = doc["transport"].get<std::string>();
    // // config.m_ip = doc["ip"].get<std::string>();
    // // config.m_control_port = std::to_string(doc["control_port"].get<int>());
    // // config.m_shell_port = std::to_string(doc["shell_port"].get<int>());
    // // config.m_stdin_port = std::to_string(doc["stdin_port"].get<int>());
    // // config.m_iopub_port = std::to_string(doc["iopub_port"].get<int>());
    // // config.m_hb_port = std::to_string(doc["hb_port"].get<int>());
    // // config.m_signature_scheme = doc.value("signature_scheme", "");



    using history_manager_ptr = std::unique_ptr<xeus::xhistory_manager>;
    history_manager_ptr hist = xeus::make_in_memory_history_manager();
    
    using interpreter_ptr = std::unique_ptr<test_kernel::test_interpreter>;
    auto interpreter = interpreter_ptr(new test_kernel::test_interpreter());
    
    std::cout<<"done\n";
    xeus::xkernel * kernel = new xeus::xkernel(config,
                         xeus::get_user_name(),
                         std::move(interpreter),
                         std::move(hist),
                         nullptr,
                         xeus::make_xserver_emscripten);
    // std::cout << "starting kernel" << std::endl;
    // kernel.start();
    // std::cout<<"done 2\n";

    // std::cout << "countdown:\n";
    // for (int i=2; i>0; --i) {
    //     std::cout << i << std::endl;
    //     std::this_thread::sleep_for (std::chrono::seconds(1));
    // }
    // std::cout << "Lift off!\n";
    // auto leak = new leak_it<interpreter_ptr>(std::move(interpreter));
    // std::cout << "Lift done!\n";
    return kernel;
}

using namespace emscripten;

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}


xeus::xserver * get_server(xeus::xkernel * kernel){
    return &kernel->get_server();
}



EMSCRIPTEN_BINDINGS(my_module) {

    function("lerp", &lerp);

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


    class_<interpreter_base_type>("xinterpreter")
        .function
        <
        std::string(
            interpreter_base_type &,
            std::string ,
            bool ,
            bool ,
            std::string,
            bool 
        )>
        ("execute_request",[](
            interpreter_base_type & self,
            const std::string& code,
            bool silent,
            bool store_history,
            std::string user_expressions_json_str,
            bool allow_stdin
        )->std::string{
            auto user_expressions = nl::json::parse(user_expressions_json_str);
            return self.execute_request(code, silent, store_history, user_expressions, allow_stdin).dump();
        })

        .function<void( interpreter_base_type &,emscripten::val)>("register_publisher", [](
            interpreter_base_type & self, emscripten::val val
        ){
            // wrap in an object
            std::function<void(const std::string&, nl::json, nl::json, xeus::buffer_sequence)>
            publisher([val](const std::string& a, nl::json b, nl::json c, xeus::buffer_sequence buffer_sequence){
                std::cout<<"size of buffer_sequence "<<buffer_sequence.size()<<"\n";
                val(a, b.dump(), c.dump(), buffer_sequence);
            });
            self.register_publisher(publisher);
        })
    ;

    class_<interpreter_type, base<interpreter_base_type>>("test_interpreter")
         .constructor<>()
    ;

    // using server_type = xeus::xserver;
    // using server_emscripten_type = xeus::xserver_emscripten;

    // class_<server_type>("xserver")
    //     .function("stop",&server_type::stop)
    //      .function("send_shell",    &server_type::send_shell)
    //      .function("send_control",  &server_type::send_control)
    //      .function("send_stdin",    &server_type::send_stdin)
    // ;




    // class_<server_emscripten_type, base<server_type>>("xserver_emscripten")
    // ;


    // class_<xeus::xkernel>("xdummy_kernel")
    //     .constructor(&make_xkernel, allow_raw_pointers())
    //     .function("start", &xeus::xkernel::start)
    //     .function("get_server", &get_server,allow_raw_pointers())
    //     //.function("get_server", &xeus::xkernel::get_server)
    // ;
}