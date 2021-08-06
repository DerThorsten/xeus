
#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_emscripten.hpp"
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

    }

    void xserver_emscripten::send_control_impl(zmq::multipart_t& message) 
    {

    }

    void xserver_emscripten::send_stdin_impl(zmq::multipart_t& message) 
    {

    }

    void xserver_emscripten::publish_impl(zmq::multipart_t& message, channel c) 
    {

    }


    void xserver_emscripten::start_impl(zmq::multipart_t& message) 
    {

    }

    void xserver_emscripten::abort_queue_impl(const listener& l, long polling_interval) 
    {

    }

    void xserver_emscripten::stop_impl() 
    {

    }

    void xserver_emscripten::update_config_impl(xconfiguration& config) const 
    {

    }


    std::unique_ptr<xserver> make_xserver_emscripten(zmq::context_t& context, const xconfiguration& config)
    {
        return std::make_unique<xserver_emscripten>(config);
    }
}


