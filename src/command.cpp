#include "command.h"

// ── Command ─────────────────────────────────────────────────────

Command Command::parse(const std::string& msg, const std::string& cmd_name, PrefixMode mode) {
    auto handle = luo9_command_create(msg.c_str(), cmd_name.c_str(), mode.mode(), mode.prefix());
    return Command(handle);
}

Command::~Command() {
    if (handle_) luo9_command_free(handle_);
}

std::string Command::name() const {
    char* s = luo9_command_get_name(handle_);
    if (!s) return "";
    std::string r(s);
    luo9_free_string(s);
    return r;
}

std::string Command::args_raw() const {
    char* s = luo9_command_get_args_raw(handle_);
    if (!s) return "";
    std::string r(s);
    luo9_free_string(s);
    return r;
}

bool Command::has_args() const { return luo9_command_has_args(handle_) == 1; }
int Command::args_count() const { return luo9_command_args_count(handle_); }

std::string Command::arg_at(int i) const {
    char* s = luo9_command_get_arg(handle_, i);
    if (!s) return "";
    std::string r(s);
    luo9_free_string(s);
    return r;
}

std::vector<std::string> Command::args() const {
    int count = args_count();
    std::vector<std::string> result;
    result.reserve(count);
    for (int i = 0; i < count; i++) {
        result.push_back(arg_at(i));
    }
    return result;
}

std::vector<std::string> Command::args_from_internal(int start) const {
    int count = args_count();
    std::vector<std::string> result;
    for (int i = start; i < count; i++) {
        result.push_back(arg_at(i));
    }
    return result;
}

CommandMatcher Command::on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) && {
    if (has_args() && arg_at(0) == expected) {
        f(args_from_internal(1));
        return CommandMatcher(*this, true);
    }
    return CommandMatcher(*this, false);
}

CommandMatcher Command::on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) const & {
    if (has_args() && arg_at(0) == expected) {
        f(args_from_internal(1));
        return CommandMatcher(*this, true);
    }
    return CommandMatcher(*this, false);
}

// ── CommandMatcher ──────────────────────────────────────────────

CommandMatcher CommandMatcher::on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) && {
    if (!matched_) {
        if (cmd_.has_args() && cmd_.arg_at(0) == expected) {
            f(cmd_.args_from_internal(1));
            matched_ = true;
        }
    }
    return std::move(*this);
}

void CommandMatcher::otherwise(std::function<void()> f) && {
    if (!matched_) {
        f();
    }
}
