#pragma once

#include <CRSLib/include/std_int.hpp>

namespace CRSLib::Debug
{
    namespace Implement::DebugImp
    {
        inline constinit bool error_happened{false};
        inline constinit const char * error_message{nullptr};
    }

    inline void set_error(const char *const str = "Error happened.") noexcept
    {
        Implement::DebugImp::error_happened = true;
        if(!Implement::DebugImp::error_message) Implement::DebugImp::error_message = str;
    }

    void error_handler() noexcept;
}
