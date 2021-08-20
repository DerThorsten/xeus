#ifndef XEUS_XEMS_INTERPRETER_HPP
#define XEUS_XEMS_INTERPRETER_HPP

#include <emscripten.h>
#include "xeus/xinterpreter.hpp"



namespace xeus
{


    template<class interpreter_t>
    class ems_interpreter : public interpreter_t
    {
    public:
        ems_interpreter()
        : interpreter_t(){

            // wrap in an object
            std::function<const nl::json&()>
            parent_header([&](){
                return m_parent_header;
            });
            this->register_parent_header(parent_header);


        }

        void set_parent_header_str(const std::string & parent_header_str)
        {
            m_parent_header = nl::json::parse(parent_header_str);
        }
        std::string get_parent_header_str()const
        {
            return m_parent_header.dump();
        }
        void set_async_input_func(emscripten::val val)
        {
            if(m_async_input_func == nullptr)
            {
                m_async_input_func = new emscripten::val(std::move(val));

                // std::cout<<"await the stuff\n";
                // emscripten::val res = val().await();
                // std::cout<<"the fucking res "<<  res.as<std::string>()<<"\n";
            }
           
        }
    private:
        nl::json m_parent_header; 
    public:
        emscripten::val * m_async_input_func;
    };

}

#endif