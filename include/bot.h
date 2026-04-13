#ifndef __BOT_H__
#define __BOT_H__

#include <cstdint>
#include <string>
#include <iostream>

#include "head.h"

extern "C" {
    /// 获取核心版本信息
    LUO9_API const char* luo9_version();
    
    /// 发送群消息
    LUO9_API int luo9_send_group_msg(uint64_t group_id, const char* msg);
    
    /// 发送私聊消息
    LUO9_API int luo9_send_private_msg(uint64_t user_id, const char* msg);
}

namespace Bot {
    std::string get_version();
    int send_private_msg(uint64_t user_id, std::string msg);
    int send_group_msg(uint64_t group_id, std::string msg);
};

#endif // __BOT_H__
