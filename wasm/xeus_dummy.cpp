/* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <memory>

#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <emscripten/bind.h>

#include "test_interpreter.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xembind.hpp"





EMSCRIPTEN_BINDINGS(my_module) {
    
    xeus::export_core();
    using interpreter_type =    test_kernel::test_interpreter;
    xeus::export_interpreter<interpreter_type>("test_interpreter");

}