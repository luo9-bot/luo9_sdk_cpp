#include "bot.h"
#include "bus.h"

std::string Bot::get_version() {
    return luo9_version();
}

void Bot::send_group_msg(uint64_t group_id, const std::string& msg) {
    std::string json = "{\"action\":{\"send_group_msg\":{\"group_id\":"
        + std::to_string(group_id)
        + ",\"message\":\"" + msg + "\"}}}";
    Bus::topic("luo9_send").publish(json);
}

void Bot::send_private_msg(uint64_t user_id, const std::string& msg) {
    std::string json = "{\"action\":{\"send_private_msg\":{\"user_id\":"
        + std::to_string(user_id)
        + ",\"message\":\"" + msg + "\"}}}";
    Bus::topic("luo9_send").publish(json);
}
