/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_XEMBIND_HPP
#define XEUS_XEMBIND_HPP

#include <string>

#include "xeus/xserver_emscripten.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xmessage.hpp"

namespace nl = nlohmann;


namespace xeus
{
    void export_server_emscripten()
    {
        using namespace emscripten;

        class_<xserver>("xserver")
        ;

        class_<xserver_emscripten,  base<xserver> >("xserver_emscripten")
            .function("notify_listener" ,     &xserver_emscripten::js_notify_listener)
            .function("register_js_callback" ,     &xserver_emscripten::register_js_callback)
        ;
    }

    nl::json json_parse(const std::string & json_str){
        return nl::json::parse(json_str);
    }

    void export_core()
    {
        using namespace emscripten;

        class_<nl::json>("nl_json")
            //.class_function("parse", &nl::json::parse)
        ;

        function("json_parse", &json_parse);

        class_<binary_buffer>("binary_buffer")
        ;
        class_<buffer_sequence>("buffer_sequence")
            .function("size", &buffer_sequence::size)
        ;


        class_<xmessage_base>("xmessage_base")
        ;

        class_<xmessage, base<xmessage_base> >("xmessage")
            .constructor<>()
        ;
        class_<xpub_message,  base<xmessage_base> >("xpub_message")
            .constructor<>()
        ;
        export_server_emscripten();
    }

    xeus::xserver * get_server(xeus::xkernel * kernel)
    {
        return &kernel->get_server();
    }

    template<class interpreter_type>
    std::unique_ptr<xkernel> make_xkernel()
    {
        xeus::xconfiguration config;

        using history_manager_ptr = std::unique_ptr<xeus::xhistory_manager>;
        history_manager_ptr hist = xeus::make_in_memory_history_manager();

        using interpreter_ptr = std::unique_ptr<interpreter_type>;
        auto interpreter = interpreter_ptr(new interpreter_type());

        auto context = xeus::make_empty_context();

        xeus::xkernel * kernel = new xeus::xkernel(config,
                             xeus::get_user_name(),
                             std::move(context),
                             std::move(interpreter),
                             xeus::make_xserver_emscripten,
                             std::move(hist),
                             nullptr
                             );
        return std::unique_ptr<xkernel>{kernel};
    }

    template<class interpreter_type>
    void export_kernel(const std::string kernel_name)
    {
        using namespace emscripten;
        class_<xkernel>(kernel_name.c_str())
            .constructor<>(&make_xkernel<interpreter_type>)
            .function("get_server", &get_server, allow_raw_pointers())
            .function("start", &xkernel::start)
        ;
    }
}

#endif