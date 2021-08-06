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


#include "test_interpreter.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_shell_main.hpp"


template<class smart_ptr>
struct leak_it
{
    leak_it(smart_ptr && ptr)
    : m_ptr(std::move(ptr))
    {

    }
    smart_ptr m_ptr;
};

int main(int argc, char* argv[])
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
    // xeus::xkernel kernel(config,
    //                      xeus::get_user_name(),
    //                      std::move(interpreter),
    //                      std::move(hist),
    //                      nullptr,
    //                      xeus::make_xserver_shell_main);
    std::cout << "starting kernel" << std::endl;
    //kernel.start();
    std::cout<<"done 2\n";

    // std::cout << "countdown:\n";
    // for (int i=2; i>0; --i) {
    //     std::cout << i << std::endl;
    //     std::this_thread::sleep_for (std::chrono::seconds(1));
    // }
    std::cout << "Lift off!\n";
    auto leak = new leak_it<interpreter_ptr>(std::move(interpreter));
    std::cout << "Lift done!\n";
    return 0;
}
