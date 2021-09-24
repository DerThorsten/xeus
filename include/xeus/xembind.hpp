

#ifndef XEUS_XEMBIND_HPP
#define XEUS_XEMBIND_HPP

// #include <emscripten.h>


#include <string>

// #include "xeus/xems_interpreter.hpp"
#include "xeus/xserver_emscripten.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xinterpreter.hpp"

namespace nl = nlohmann;





namespace xeus
{


    void export_server_emscripten()
    {
        using namespace emscripten;

        class_<xserver>("xserver")
        ;

        class_<xserver_emscripten,  base<xserver> >("xserver_emscripten")
            // .function("send_shell_json_str", &xserver_emscripten::send_shell_json_str)
            // .function("send_control_json_str", &xserver_emscripten::send_control_json_str)
            // .function("send_stdin_json_str", &xserver_emscripten::send_stdin_json_str)

            .function("notify_listener" ,     &xserver_emscripten::js_notify_listener)
            // .function("notify_shell_listener" ,     &xserver_emscripten::js_notify_shell_listener)
            // .function("notify_control_listener" ,   &xserver_emscripten::js_notify_control_listener)
            // .function("notify_stdin_listener" ,     &xserver_emscripten::js_notify_stdin_listener)
            .function("register_js_callback" ,     &xserver_emscripten::register_js_callback)
            // .template function<void( xserver_emscripten&,emscripten::val)>("register_js_callback", [](
            //     xserver_emscripten& self, emscripten::val val
            // ){
            //     // // wrap in an object
            //     // std::function<void(const std::string&, nl::json, nl::json)>
            //     // stdin_sender([val](const std::string& a, nl::json b, nl::json c){
            //     //     val(a, b.dump(), c.dump());
            //     // });
            //     self.register_js_callback(stdin_sender);
            // })

        ;


    }

    void export_core()
    {
        using namespace emscripten;

        class_<nl::json>("nl_json")
        ;

        export_server_emscripten();

    }



    xeus::xserver * get_server(xeus::xkernel * kernel){
        return &kernel->get_server();
    }


    template<class interpreter_type>
    xkernel * make_xkernel()
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
        return kernel;
    }

    template<class interpreter_type>
    void export_kernel(const std::string kernel_name)
    {
        using namespace emscripten;
        class_<xkernel>(kernel_name.c_str())
            .constructor<>(&make_xkernel<interpreter_type>, allow_raw_pointers())
            .function("get_server", &get_server, allow_raw_pointers())
            .function("start", &xkernel::start)
        ;
    }
}

#endif