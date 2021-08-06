/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <random>
#include <iostream>

#include "xeus/xkernel.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xhistory_manager.hpp"
#include "xeus/xcontrol_messenger.hpp"
#include "xkernel_core.hpp"
#include "xlogger_impl.hpp"

#if (defined(__linux__) || defined(__unix__))
#define LINUX_PLATFORM
#elif (defined(_WIN32) || defined(_WIN64))
#define WINDOWS_PLATFORM
#elif defined(__APPLE__)
#define APPLE_PLATFORM
#endif

#if (defined(LINUX_PLATFORM) || defined(APPLE_PLATFORM))
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif (defined(_WIN32) || defined(_WIN64))
#include <windows.h>
#include <Lmcons.h>
#endif

namespace xeus
{
    std::string get_user_name()
    {
#if ((defined(LINUX_PLATFORM) || defined(APPLE_PLATFORM))  && !defined(EMSCRIPTEN))
        struct passwd* pws;
        std::cout<<"geteuid\n";
        auto euid = geteuid();
        std::cout<<"geteuid done\n";
        std::cout<<"getpwuid\n";
        pws = getpwuid(euid);
        std::cout<<"getpwuid done\n";
        if (pws != nullptr)
        {
            std::string res = pws->pw_name;
            return res;
        }
        else
        {
            const char* user = std::getenv("USER");
            if (user != nullptr)
            {
                std::string res = user;
                return res;
            }
            else
            {
                return "unspecified user";
            }
        }
#elif defined(WINDOWS_PLATFORM)
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        GetUserName(username, &username_len);
        return username;
#else
        return "unspecified user";
#endif
        std::cout<<"got user name\n";
    }

    xkernel::xkernel(const xconfiguration& config,
                     const std::string& user_name,
                     interpreter_ptr interpreter,
                     history_manager_ptr history_manager,
                     logger_ptr logger,
                     server_builder sbuilder,
                     debugger_builder dbuilder,
                     nl::json debugger_config,
                     nl::json::error_handler_t eh)
        : m_config(config)
        , m_user_name(user_name)
        , p_interpreter(std::move(interpreter))
        , p_history_manager(std::move(history_manager))
        , p_logger(std::move(logger))
        , m_server_builder(sbuilder)
        , m_debugger_builder(dbuilder)
        , m_debugger_config(debugger_config)
        , m_error_handler(eh)
    {
        std::cout<<"init xkernel"<<std::endl;
        init();
        std::cout<<"init xkernel done\n";
    }

    xkernel::xkernel(const std::string& user_name,
                     interpreter_ptr interpreter,
                     history_manager_ptr history_manager,
                     logger_ptr logger,
                     server_builder sbuilder,
                     debugger_builder dbuilder,
                     nl::json debugger_config,
                     nl::json::error_handler_t eh)
        : m_user_name(user_name)
        , p_interpreter(std::move(interpreter))
        , p_history_manager(std::move(history_manager))
        , p_logger(std::move(logger))
        , m_server_builder(sbuilder)
        , m_debugger_builder(dbuilder)
        , m_debugger_config(debugger_config)
        , m_error_handler(eh)
    {
        init();
    }

    xkernel::~xkernel()
    {
        std::cout<<"xkernel destructor\n";
    }

    void xkernel::init()
    {
        std::cout<<"new_xguid"<<std::endl;
        m_kernel_id = new_xguid();
        std::cout<<"new_xguid"<<std::endl;
        m_session_id = new_xguid();

        if (m_config.m_key.size() == 0)
        {
            m_config.m_key = new_xguid();
        }

        std::cout<<"auth"<<std::endl;
        using authentication_ptr = xkernel_core::authentication_ptr;
        authentication_ptr auth = make_xauthentication(m_config.m_signature_scheme, m_config.m_key);

        if(p_logger == nullptr || std::getenv("XEUS_LOG") == nullptr)
        {
            p_logger = std::make_unique<xlogger_nolog>();
        }
        std::cout<<"build server"<<std::endl;
        p_server = m_server_builder(m_context, m_config);

        std::cout<<"update_config"<<std::endl;
        p_server->update_config(m_config);

        std::cout<<"build debugger"<<std::endl;
        p_debugger = m_debugger_builder(m_context, m_config, m_user_name, m_session_id, m_debugger_config);

        std::cout<<"p_core"<<std::endl;
        p_core = std::make_unique<xkernel_core>(m_kernel_id,
                                                m_user_name,
                                                m_session_id,
                                                std::move(auth),
                                                p_logger.get(),
                                                p_server.get(),
                                                p_interpreter.get(),
                                                p_history_manager.get(),
                                                p_debugger.get(),
                                                m_error_handler);
        std::cout<<"get_control_messenger"<<std::endl;
        xcontrol_messenger& messenger = p_server->get_control_messenger();

        if(p_debugger != nullptr)
        {
            p_debugger->register_control_messenger(messenger);
        }

        std::cout<<"register_control_messenger"<<std::endl;
        p_interpreter->register_control_messenger(messenger);
        std::cout<<"register_history_manager"<<std::endl;
        p_interpreter->register_history_manager(*p_history_manager);
        std::cout<<"configure"<<std::endl;
        p_interpreter->configure();
        std::cout<<"configure done"<<std::endl;

    }

    void xkernel::start()
    {
        std::cout<<"xkernel::start()\n";
        zmq::multipart_t start_msg = p_core->build_start_msg();
        std::cout<<"server start\n";
        p_server->start(start_msg);
        std::cout<<"xkernel::start() done\n";
    }

    const xconfiguration& xkernel::get_config()
    {
        return m_config;
    }

    xserver& xkernel::get_server()
    {
        return *p_server;
    }
}
