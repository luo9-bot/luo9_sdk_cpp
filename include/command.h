#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <unordered_map>

struct CommandHandle;

namespace luo9 { namespace command {

class PrefixMode {
public:
    static PrefixMode Required(char c) { return PrefixMode(0, c); }
    static PrefixMode Optional(char c) { return PrefixMode(1, c); }
    static PrefixMode None() { return PrefixMode(2, 0); }

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

    Command(Command&& other);
    Command& operator=(Command&& other);

    std::string name() const;
    std::string args_raw() const;
    bool empty() const { return !handle_; }
    bool has_args() const;
    int args_count() const;
    std::string arg_at(int i) const;
    std::vector<std::string> args() const;

    CommandMatcher on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) &&;
    CommandMatcher on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) const &;

private:
    friend class CommandMatcher;
    CommandHandle* handle_;
    explicit Command(CommandHandle* h);
    Command(const Command&);
    Command& operator=(const Command&);

    std::vector<std::string> args_from_internal(int start) const;
};

class CommandMatcher {
public:
    CommandMatcher(CommandMatcher&& other);
    CommandMatcher& operator=(CommandMatcher&& other);

    CommandMatcher on(const std::string& expected, std::function<void(const std::vector<std::string>&)> f) &&;

    CommandMatcher on_pattern(const std::string& pattern,
        std::function<void(const std::unordered_map<std::string, std::string>&,
                           const std::vector<std::string>&)> f) &&;

    void otherwise(std::function<void()> f) &&;

private:
    friend class Command;
    const Command& cmd_;
    bool matched_;
    CommandMatcher(const Command& cmd, bool matched);
};

Command parse(const std::string& msg, const std::string& cmd_name, PrefixMode mode);

}} // namespace luo9::command

#endif
