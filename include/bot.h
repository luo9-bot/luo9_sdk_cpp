#ifndef __BOT_H__
#define __BOT_H__

#include <cstdint>
#include <string>

#include "head.h"

extern "C" {
    /// 获取核心版本信息
    LUO9_API const char* luo9_version();
}

namespace Bot {
    /// 获取核心版本
    std::string get_version();

    /// 发送群消息（基于 bus 总线，fire-and-forget）
    void send_group_msg(uint64_t group_id, const std::string& msg);

    /// 发送私聊消息（基于 bus 总线，fire-and-forget）
    void send_private_msg(uint64_t user_id, const std::string& msg);
};

#endif // __BOT_H__
