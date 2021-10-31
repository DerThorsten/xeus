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
#include <iostream>

#include "xeus/xserver_emscripten.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xmessage.hpp"

namespace nl = nlohmann;
namespace ems = emscripten;

namespace xeus
{
    void export_server_emscripten()
    {
        //using namespace emscripten;

        ems::class_<xserver>("xserver")
        ;

        ems::class_<xserver_emscripten,  ems::base<xserver> >("xserver_emscripten")
            .function("notify_listener" ,     &xserver_emscripten::js_notify_listener)
            .function("register_js_callback" ,     &xserver_emscripten::register_js_callback)
        ;
    }

    nl::json json_parse(const std::string & json_str){
        return nl::json::parse(json_str);
    }

    binary_buffer binary_buffer_from_string(const std::string & data)
    {
        return binary_buffer(data.begin(), data.end());
    }


    ems::val get_bytes(binary_buffer & buffer) {
        return ems::val(ems::typed_memory_view(buffer.size(), buffer.data()));
    }

    void export_core()
    {
        using namespace emscripten;

        class_<nl::json>("nl_json")
            //.class_function("parse", &nl::json::parse)
        ;

        function("json_parse", &json_parse);
        function("binary_buffer_from_string", &binary_buffer_from_string);

        class_<binary_buffer>("binary_buffer")
            .function("get_mem_view", &get_bytes)
        ;
        class_<buffer_sequence>("buffer_sequence")
            .constructor<>()
            .function("size", &buffer_sequence::size)
            .function("push_back", std::function<void(buffer_sequence&, const binary_buffer &)>([](buffer_sequence& self, const binary_buffer & b){
                self.push_back(b);
            }))

            .function("view", std::function<val(buffer_sequence&)>([](buffer_sequence& self){
                ems::val return_array = ems::val::array();
                for(auto &buffer : self)
                {
                    ems::val mem_view = ems::val(ems::typed_memory_view(buffer.size(), buffer.data()));
                    return_array.call<void>("push", mem_view);
                }
                return return_array;
            }))
            .function("copy", std::function<val(buffer_sequence&)>([](buffer_sequence& self){
                ems::val return_array = ems::val::array();
                for(auto &buffer : self)
                {
                    ems::val mem_view = ems::val(ems::typed_memory_view(buffer.size(), buffer.data()));
                    ems::val mem_copuy = val::global("Int8Array").new_(mem_view);
                    return_array.call<void>("push", mem_view);
                }
                return return_array;
            }))
            .function("from_js", std::function<void(buffer_sequence&, ems::val)>([](buffer_sequence& self, ems::val buffers){
                const unsigned n_buffers = buffers["length"].as<unsigned>();
                self.resize(n_buffers);

                std::vector<ems::val> buffers_vec = ems::vecFromJSArray<ems::val>(buffers);
                for(std::size_t i=0; i<buffers_vec.size(); ++i)
                {
                    // the typed array an be of any type
                    ems::val js_array = buffers_vec[i];

                    // data we need to convert js_array into an js Uint8Arra
                    ems::val js_array_buffer = js_array["buffer"].as<ems::val>();
                    ems::val byteOffset = js_array["byteOffset"].as<ems::val>();
                    unsigned length = buffers_vec[i]["length"].as<unsigned>();
                    unsigned int bytesPerElement  = js_array["BYTES_PER_ELEMENT"].as<unsigned>();

                    // convert js_array into an js Uint8Array
                    ems::val js_uint8array = val::global("Uint8Array").new_(js_array_buffer, byteOffset, length * bytesPerElement);

                    // resize array on c++ size
                    const unsigned buffer_size = js_uint8array["length"].as<unsigned>();
                    self[i].resize(buffer_size);

    
                    // from js to c++
                    val heap = val::module_property("HEAPU8");
                    val memory = heap["buffer"];
                    val memoryView = js_uint8array["constructor"].new_(memory, reinterpret_cast<uintptr_t>(self[i].data()), buffer_size);
                    memoryView.call<void>("set", js_uint8array);


                }
            }), allow_raw_pointers())
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