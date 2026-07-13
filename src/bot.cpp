#include "bot.h"
#include "bus.h"

extern "C" {
    const char* luo9_version();
}

namespace luo9 { namespace bot {

std::string get_version() {
    return luo9_version();
}

void send_group_msg(uint64_t group_id, const std::string& msg) {
    std::string json = "{\"action\":{\"send_group_msg\":{\"group_id\":"
        + std::to_string(group_id)
        + ",\"message\":\"" + msg + "\"}}}";
    bus::topic("luo9_send").publish(json);
}

void send_private_msg(uint64_t user_id, const std::string& msg) {
    std::string json = "{\"action\":{\"send_private_msg\":{\"user_id\":"
        + std::to_string(user_id)
        + ",\"message\":\"" + msg + "\"}}}";
    bus::topic("luo9_send").publish(json);
}

}} // namespace luo9::bot
