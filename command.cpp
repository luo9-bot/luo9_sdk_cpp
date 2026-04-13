#include "command.h"

Command Command::parse(const std::string& msg, const std::string& cmd_name, PrefixMode mode) {
    auto handle = luo9_command_create(msg.c_str(), cmd_name.c_str(), mode.mode(), mode.prefix());
    return Command(handle);
}

Command::~Command() {
    if (handle_) luo9_command_free(handle_);
}

std::string Command::name() const {
    char* s = luo9_command_get_name(handle_);
    std::string r = s;
    luo9_free_string(s);
    return r;
}

std::string Command::args_raw() const {
    char* s = luo9_command_get_args_raw(handle_);
    std::string r = s;
    luo9_free_string(s);
    return r;
}

bool Command::has_args() const { return luo9_command_has_args(handle_) == 1; }
int Command::args_count() const { return luo9_command_args_count(handle_); }
std::string Command::arg_at(int i) const {
    char* s = luo9_command_get_arg(handle_, i);
    if (!s) return "";
    std::string r = s;
    luo9_free_string(s);
    return r;
}

