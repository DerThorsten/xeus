

#ifndef XEUS_XEMBIND_HPP
#define XEUS_XEMBIND_HPP

// #include <emscripten.h>


#include <string>

#include "xeus/xems_interpreter.hpp"
#include "xeus/xinterpreter.hpp"
#include "xwrap_zmq.hpp"
#include "xwrap_zmq_addon.hpp"

namespace nl = nlohmann;





namespace xeus
{

    void export_core()
    {
        using namespace emscripten;

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


    }



    template<class interpreter_type>
    void export_interpreter(const std::string & interpreter_name)
    {
        using namespace emscripten;
        using ems_interpreter_type =    ems_interpreter<interpreter_type>;

        class_<ems_interpreter_type>(interpreter_name.c_str())
            .constructor<>()

            ////////////////////////////////////////////////////////////////////////////////////
            . template function
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

                //int res = get_input();

                auto user_expressions = nl::json::parse(user_expressions_json_str);
                auto dumped =  self.execute_request(code, silent, store_history, user_expressions, allow_stdin).dump();
                return dumped;
            })
            ////////////////////////////////////////////////////////////////////////////////////
            . template function
            <
            std::string(
                ems_interpreter_type&,
                std::string ,
                int
            )>
            ("complete_request",[](
                ems_interpreter_type& self,
                const std::string & code,
                int cursor_pos
            )->std::string{
                return self.complete_request(code, cursor_pos).dump();
            })
            .template function<void( ems_interpreter_type&,emscripten::val)>("register_publisher", [](
                ems_interpreter_type& self, emscripten::val val
            ){
                // wrap in an object
                std::function<void(const std::string&, nl::json, nl::json, xeus::buffer_sequence)>
                publisher([val](const std::string& a, nl::json b, nl::json c, xeus::buffer_sequence buffer_sequence){
                    //std::cout<<"size of buffer_sequence "<<buffer_sequence.size()<<"\n";
                    val(a, b.dump(), c.dump(), buffer_sequence);
                });
                self.register_publisher(publisher);
            })
            ////////////////////////////////////////////////////////////////////////////////////
            . template function
            <
            std::string(
                ems_interpreter_type&
            )>
            ("kernel_info_request",[](
                ems_interpreter_type& self
            )->std::string{
                return self.kernel_info_request().dump();
            })
            ////////////////////////////////////////////////////////////////////////////////////
            .template function<void( ems_interpreter_type&,emscripten::val)>("register_publisher", [](
                ems_interpreter_type& self, emscripten::val val
            ){
                // wrap in an object
                std::function<void(const std::string&, nl::json, nl::json, xeus::buffer_sequence)>
                publisher([val](const std::string& a, nl::json b, nl::json c, xeus::buffer_sequence buffer_sequence){
                    val(a, b.dump(), c.dump(), buffer_sequence);
                });
                self.register_publisher(publisher);
            })
            ////////////////////////////////////////////////////////////////////////////////////
            .template function<void( ems_interpreter_type&,emscripten::val)>("register_stdin_sender", [](
                ems_interpreter_type& self, emscripten::val val
            ){
                // wrap in an object
                std::function<void(const std::string&, nl::json, nl::json)>
                stdin_sender([val](const std::string& a, nl::json b, nl::json c){
                    val(a, b.dump(), c.dump());
                });
                self.register_stdin_sender(stdin_sender);
            })
            ////////////////////////////////////////////////////////////////////////////////////
            .template function<void( ems_interpreter_type&,emscripten::val)>("register_input_handler", [](
                ems_interpreter_type& self, emscripten::val val
            ){
                // wrap in an object
                std::function<void(const std::string&)>
                input_handler([val](const std::string& a){
                    val(a);
                });
                self.register_input_handler(input_handler);
            })
            ////////////////////////////////////////////////////////////////////////////////////
            .function("set_async_input_func", &ems_interpreter_type::set_async_input_func)
            .function("get_parent_header_str", &ems_interpreter_type::get_parent_header_str)
            .function("set_parent_header_str", &ems_interpreter_type::set_parent_header_str)



        ;   
    }


}

#endif