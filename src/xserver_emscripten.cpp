
#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_emscripten.hpp"

#include <iostream>

#include <emscripten.h>


namespace xeus
{



    void json_str_to_multipart(const std::string & json_str_wire_msg, zmq::multipart_t& wire_msg)
    {
        auto json_wire_msg = nl::json::parse(json_str_wire_msg);

        nl::json json_header, json_parent_header, json_metadata, json_content;

        // to make it save
        json_header = json_wire_msg["header"];
        json_parent_header = json_wire_msg["parent_header"];
        json_metadata = json_wire_msg["metadata"];
        json_content = json_wire_msg["content"];


        // to take error handler of xkernel
        nl::json::error_handler_t error_handler = nl::json::error_handler_t::strict;

        zmq::message_t header = write_zmq_message(json_header, error_handler);
        zmq::message_t parent_header = write_zmq_message(json_parent_header, error_handler);
        zmq::message_t metadata = write_zmq_message(json_metadata, error_handler);
        zmq::message_t content = write_zmq_message(json_content, error_handler);
        zmq::message_t signature;

        wire_msg.add(std::move(signature));
        wire_msg.add(std::move(header));
        wire_msg.add(std::move(parent_header));
        wire_msg.add(std::move(metadata));
        wire_msg.add(std::move(content));

        // todo handler buffers
    }


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
    :    p_messenger(new xtrivial_emscripten_messenger(this)),
    p_js_callback(nullptr)
    {

    }

    xserver_emscripten::~xserver_emscripten()
    {
        if(p_js_callback!=nullptr)
        {
            delete p_js_callback;
        }
    }


    void xserver_emscripten::send_to_js(const std::string type, zmq::multipart_t& wire_msg, channel c)
    {
        if(p_js_callback)
        {
            // this is mostly a  reimpl of what is in xmessage but
            // witthout the auth part

            zmq::message_t zmq_signature = wire_msg.pop();
            zmq::message_t zmq_header = wire_msg.pop();
            zmq::message_t zmq_parent_header = wire_msg.pop();
            zmq::message_t zmq_metadata = wire_msg.pop();
            zmq::message_t zmq_content = wire_msg.pop();


            auto buffers = nl::json::array();
            nl::json json_header, json_parent_header, json_metadata, json_content;

            parse_zmq_message(zmq_header,           json_header);
            parse_zmq_message(zmq_parent_header,    json_parent_header);
            parse_zmq_message(zmq_metadata,         json_metadata);
            parse_zmq_message(zmq_content,          json_content);

            while (!wire_msg.empty())
            {
                zmq::message_t zmq_buffer = wire_msg.pop();
                nl::json json_buffer;
                parse_zmq_message(zmq_buffer, json_buffer);
                buffers.push_back(json_buffer);
            }

            // build overall json object
            nl::json j;
            
            // the actual msg
            j["header"] = json_header;
            j["parent_header"] = json_parent_header;
            j["metadata"] = json_metadata;
            j["content"] = json_content;
            j["buffers"] = buffers;

            // the callback
            (*p_js_callback)(type, int(c), j.dump());

        }
    }


    void xserver_emscripten::js_notify_shell_listener(const std::string & json_str)
    {
        zmq::multipart_t message;
        json_str_to_multipart(json_str, message);
        this->notify_shell_listener(message);                
    }
    void xserver_emscripten::js_notify_control_listener(const std::string & json_str)
    {
        zmq::multipart_t message;
        json_str_to_multipart(json_str, message);
        this->notify_control_listener(message);        
    }
    void xserver_emscripten::js_notify_stdin_listener(const std::string & json_str)
    {
        zmq::multipart_t message;
        json_str_to_multipart(json_str, message);
        this->notify_stdin_listener(message);        
    }




    xcontrol_messenger& xserver_emscripten::get_control_messenger_impl() 
    {
        return *p_messenger;
    }

    void xserver_emscripten::send_shell_impl(zmq::multipart_t& message) 
    {
        std::cout<<"send_shell_impl\n";
        this->send_to_js("shell", message); 
    }

    void xserver_emscripten::send_control_impl(zmq::multipart_t& message) 
    {
        std::cout<<"send_control_impl\n";
        this->send_to_js("control", message);        
    }

    void xserver_emscripten::send_stdin_impl(zmq::multipart_t& message) 
    {
        std::cout<<"send_stdin_impl\n";
        this->send_to_js("sdtin", message);        
    }

    void xserver_emscripten::publish_impl(zmq::multipart_t& message, channel c) 
    {
        std::cout<<"publish_impl\n";
        this->send_to_js("publish", message, c);        
    }


    void xserver_emscripten::register_js_callback(emscripten::val callback)
    {
        if(p_js_callback == nullptr)
        {
            p_js_callback = new emscripten::val(callback);
        }
        else
        {
            throw std::runtime_error("callback is already registerd");
        }
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


