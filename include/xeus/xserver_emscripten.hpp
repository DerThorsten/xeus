#ifndef XEUS_XEMSCRIPTEN_SERVER_HPP
#define XEUS_XEMSCRIPTEN_SERVER_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"

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


        xserver_emscripten(const xconfiguration& config);

        ~xserver_emscripten() = default;

        using xserver::notify_internal_listener;

    protected:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void send_stdin_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message, channel c) override;

        void start_impl(zmq::multipart_t& message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;



        using trivial_messenger_ptr = std::unique_ptr<xtrivial_emscripten_messenger>;
        trivial_messenger_ptr p_messenger;

    };
}

#endif
