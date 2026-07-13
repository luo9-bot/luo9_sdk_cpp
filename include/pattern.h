#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace luo9 { namespace pattern {

class Pattern {
public:
    explicit Pattern(const std::string& tmpl);
    std::optional<std::unordered_map<std::string, std::string>> match(const std::string& s) const;

private:
    enum class PartKind { Literal, Capture };
    struct Part {
        PartKind kind;
        std::string value;
        Part(PartKind k, const std::string& v) : kind(k), value(v) {}
    };
    std::vector<Part> parts_;
    const std::string* next_literal_after(size_t index) const;
};

}} // namespace luo9::pattern

#endif
