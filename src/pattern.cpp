#include "pattern.h"

namespace luo9 { namespace pattern {

Pattern::Pattern(const std::string& tmpl) {
    std::string rest = tmpl;
    size_t pos = 0;

    while (pos < rest.size()) {
        size_t start = rest.find('{', pos);
        if (start == std::string::npos) {
            if (pos < rest.size()) {
                parts_.push_back(Part(PartKind::Literal, rest.substr(pos)));
            }
            break;
        }

        if (start > pos) {
            parts_.push_back(Part(PartKind::Literal, rest.substr(pos, start - pos)));
        }

        size_t end = rest.find('}', start + 1);
        if (end == std::string::npos) {
            parts_.push_back(Part(PartKind::Literal, "{"));
            pos = start + 1;
        } else {
            std::string capture_name = rest.substr(start + 1, end - start - 1);
            parts_.push_back(Part(PartKind::Capture, capture_name));
            pos = end + 1;
        }
    }
}

std::optional<std::unordered_map<std::string, std::string>> Pattern::match(const std::string& s) const {
    std::unordered_map<std::string, std::string> captures;
    std::string remaining = s;

    for (size_t i = 0; i < parts_.size(); ++i) {
        const auto& part = parts_[i];

        if (part.kind == PartKind::Literal) {
            if (remaining.size() < part.value.size() ||
                remaining.substr(0, part.value.size()) != part.value) {
                return std::nullopt;
            }
            remaining = remaining.substr(part.value.size());
        } else {
            const std::string* next_lit = next_literal_after(i);
            size_t end_pos;
            if (next_lit) {
                end_pos = remaining.find(*next_lit);
                if (end_pos == std::string::npos) {
                    return std::nullopt;
                }
            } else {
                end_pos = remaining.size();
            }

            captures[part.value] = remaining.substr(0, end_pos);
            remaining = remaining.substr(end_pos);
        }
    }

    if (remaining.empty()) {
        return captures;
    }
    return std::nullopt;
}

const std::string* Pattern::next_literal_after(size_t index) const {
    for (size_t i = index + 1; i < parts_.size(); ++i) {
        if (parts_[i].kind == PartKind::Literal) {
            return &parts_[i].value;
        }
    }
    return nullptr;
}

}} // namespace luo9::pattern
