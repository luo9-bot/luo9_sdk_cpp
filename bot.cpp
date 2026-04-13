#include "bot.h"

std::string Bot::get_version(){
    return luo9_version();
}

int Bot::send_private_msg(uint64_t user_id, std::string msg){
    return luo9_send_private_msg(user_id, msg.c_str());
}

int Bot::send_group_msg(uint64_t group_id, std::string msg){
    return luo9_send_group_msg(group_id, msg.c_str());
}

