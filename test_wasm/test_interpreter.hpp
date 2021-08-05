/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef TEST_INTERPRETER_HPP
#define TEST_INTERPRETER_HPP

#include "xeus/xinterpreter.hpp"

namespace test_kernel
{
    class test_interpreter: public xeus::xinterpreter
    {

    public:

        test_interpreter() = default;
        virtual ~test_interpreter() {
            std::cout<<"destruct interpreter"<<std::endl;
        }

    private:

        void configure_impl() ;

        nl::json execute_request_impl(int execution_counter,
                                      const std::string& code,
                                      bool silent,
                                      bool store_history,
                                      nl::json user_expressions,
                                      bool allow_stdin) ;

        nl::json complete_request_impl(const std::string& code,
                                       int cursor_pos) ;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) ;

        nl::json is_complete_request_impl(const std::string& code) ;

        nl::json kernel_info_request_impl() ;

        void shutdown_request_impl() ;
    };
}

#endif
