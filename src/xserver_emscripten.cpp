
#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_emscripten.hpp"

#include <iostream>

#include <emscripten/websocket.h>
#include <emscripten.h>


namespace xeus
{


    xtrivial_emscripten_messenger::xtrivial_emscripten_messenger(xserver_emscripten* server)
    : p_server(server)
    {

    }
    xtrivial_emscripten_messenger::~xtrivial_emscripten_messenger()
    {

    }
    nl::json xtrivial_emscripten_messenger::send_to_shell_impl(const nl::json& message)
    {
        zmq::multipart_t wire_msg(message.dump());
        zmq::multipart_t wire_rep = p_server->notify_internal_listener(wire_msg);
        return nl::json::parse(wire_rep.popstr());
    }

   xserver_emscripten::xserver_emscripten(const xconfiguration& config)
   : p_messenger(new xtrivial_emscripten_messenger(this))
   {

   }


    xcontrol_messenger& xserver_emscripten::get_control_messenger_impl() 
    {
        return *p_messenger;
    }

    void xserver_emscripten::send_shell_impl(zmq::multipart_t& message) 
    {
        std::cout<<"send_shell_impl\n";
        this->notify_shell_listener(message);
    }

    void xserver_emscripten::send_control_impl(zmq::multipart_t& message) 
    {
        std::cout<<"send_control_impl\n";
        this->notify_control_listener(message);
    }

    void xserver_emscripten::send_stdin_impl(zmq::multipart_t& message) 
    {
        std::cout<<"send_stdin_impl\n";
        this->notify_stdin_listener(message);
    }

    void xserver_emscripten::publish_impl(zmq::multipart_t& message, channel c) 
    {
        std::cout<<"publish_impl\n";
    }




    void RenderLoopCallback(void* arg)
    {
        static_cast<xserver_emscripten*>(arg)->loop_func();
    }

    void xserver_emscripten::loop_func()
    {
        //std::cout<<"loop func\n";
    } 


    void xserver_emscripten::start_impl(zmq::multipart_t& message) 
    {
        std::cout<<"start_impl\n";


        // this->register_shell_listener(listener([](zmq::multipart_t & message)->void{
        //     std::cout<<"hello from shell_listener \n";
        // }));
        // this->register_control_listener(listener([](zmq::multipart_t & message)->void{
        //     std::cout<<"hello from control_listener \n";
        // }));
        // this->register_stdin_listener(listener([](zmq::multipart_t & message)->void{
        //     std::cout<<"hello from stdin_listener \n";
        // }));
        // this->register_internal_listener(internal_listener([](zmq::multipart_t & message)->void{
        //     std::cout<<"internal_listener \n";
        // }));


       

        std::cout<<"the message is "<<message.str()<<"\n";
        // emscripten_set_main_loop_arg(&RenderLoopCallback, this, -1, 1);
    }

    void xserver_emscripten::abort_queue_impl(const listener& l, long polling_interval) 
    {

    }

    void xserver_emscripten::stop_impl() 
    {
        std::cout<<"stop_impl\n";
    }

    void xserver_emscripten::update_config_impl(xconfiguration& config) const 
    {
        std::cout<<"update_config_impl\n";
    }


    std::unique_ptr<xserver> make_xserver_emscripten(zmq::context_t& context, const xconfiguration& config)
    {
        return std::make_unique<xserver_emscripten>(config);
    }
}


