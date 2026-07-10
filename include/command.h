#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <string>
#include <vector>
#include <functional>

#include "head.h"

extern "C" {
    struct CommandHandle;
    LUO9_API CommandHandle* luo9_command_create(const char* msg, const char* cmd_name, int mode, char prefix_char);
    LUO9_API void luo9_command_free(CommandHandle* handle);
    LUO9_API char* luo9_command_get_name(CommandHandle* handle);
    LUO9_API char* luo9_command_get_args_raw(CommandHandle* handle);
    LUO9_API int luo9_command_has_args(CommandHandle* handle);
    LUO9_API int luo9_command_args_count(CommandHandle* handle);
    LUO9_API char* luo9_command_get_arg(CommandHandle* handle, int index);
    LUO9_API void luo9_free_string(char* ptr);
}

class PrefixMode {
public:
    static PrefixMode Required(char c) { return {0, c}; }
    static PrefixMode Optional(char c) { return {1, c}; }
    static PrefixMode None() { return {2, 0}; }

    int mode() const { return mode_; }
    char prefix() const { return prefix_; }

private:
    int mode_;
    char prefix_;
    PrefixMode(int m, char c) : mode_(m), prefix_(c) {}
};

class CommandMatcher;

class Command {
public:
    static Command parse(const std::string& msg, const std::string& cmd_name, PrefixMode mode);

    ~Command();

    Command(Command&&) = default;
    Command& operator=(Command&&) = default;

    std::string name() const;
    std::string args_raw() const;
    bool empty() const { return !handle_; }
    bool has_args() const;
    int args_count() const;
    std::string arg_at(int i) const;
    std::vector<std::string> args() const;

    /// 链式子命令匹配：如果 arg_at(0) == expected，执行回调
    CommandMatcher on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) &&;
    CommandMatcher on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) const &;

private:
    friend class CommandMatcher;
    CommandHandle* handle_;
    explicit Command(CommandHandle* h) : handle_(h) {}
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    /// 从指定索引开始的剩余参数
    std::vector<std::string> args_from_internal(int start) const;
};

class CommandMatcher {
public:
    CommandMatcher(CommandMatcher&&) = default;
    CommandMatcher& operator=(CommandMatcher&&) = default;

    /// 继续链式匹配（仅在未匹配时生效）
    CommandMatcher on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) &&;

    /// 兜底处理（仅在未匹配时生效）
    void otherwise(std::function<void()> f) &&;

private:
    friend class Command;
    const Command& cmd_;
    bool matched_;
    CommandMatcher(const Command& cmd, bool matched) : cmd_(cmd), matched_(matched) {}
};

#endif // __COMMAND_H__
