#ifndef XEUS_XEMSCRIPTEN_SERVER_HPP
#define XEUS_XEMSCRIPTEN_SERVER_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"

#include <emscripten/bind.h>

namespace xeus
{

    class xserver_emscripten;

    class XEUS_API xtrivial_emscripten_messenger : public xcontrol_messenger
    {
    public:
        
        explicit xtrivial_emscripten_messenger(xserver_emscripten* server);
        virtual ~xtrivial_emscripten_messenger ();

    private:

        nl::json send_to_shell_impl(const nl::json& message) override;

        xserver_emscripten* p_server;
    };

    class XEUS_API xserver_emscripten : public xserver
    {
    public:

        // using js_publisher_type = std::function<void(const std::string&, nl::json, nl::json, buffer_sequence)>;
 

        xserver_emscripten(const xconfiguration& config);

        ~xserver_emscripten();


        void loop_func();

        // needs to be public for embind
        using xserver::notify_shell_listener;
        using xserver::notify_control_listener;
        using xserver::notify_stdin_listener;
        using xserver::notify_internal_listener;


        // void send_shell_json_str(const std::string & message);
        // void send_control_json_str(const std::string & message);
        // void send_stdin_json_str(const std::string & message);

        void register_js_callback(emscripten::val callback);
    protected:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void send_stdin_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message, channel c) override;



        void send_to_js(const std::string type, zmq::multipart_t& message, channel c = channel::SHELL);

        void start_impl(zmq::multipart_t& message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;



        using trivial_messenger_ptr = std::unique_ptr<xtrivial_emscripten_messenger>;
        trivial_messenger_ptr p_messenger;

        emscripten::val * p_js_callback;
    };


    XEUS_API
    std::unique_ptr<xserver> make_xserver_emscripten(zmq::context_t& context, const xconfiguration& config);
}

#endif
