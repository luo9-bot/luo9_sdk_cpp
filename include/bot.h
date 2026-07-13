#ifndef __BOT_H__
#define __BOT_H__

#include <cstdint>
#include <string>

namespace luo9 { namespace bot {

std::string get_version();
void send_group_msg(uint64_t group_id, const std::string& msg);
void send_private_msg(uint64_t user_id, const std::string& msg);

}} // namespace luo9::bot

#endif
