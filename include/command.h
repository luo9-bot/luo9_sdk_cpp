#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <string>
#include <memory>

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

private:
    CommandHandle* handle_;
    explicit Command(CommandHandle* h) : handle_(h) {}
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;
};

#endif // __COMMAND_H__
