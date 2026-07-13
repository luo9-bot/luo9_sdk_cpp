#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <string>
#include <vector>
#include <cstdint>

namespace luo9 { namespace message {

class MsgBuilder {
public:
    MsgBuilder& txt(const std::string& text);
    MsgBuilder& at(uint64_t user_id);
    MsgBuilder& image(const std::string& path);
    MsgBuilder& endl();
    std::string build() const;

private:
    std::vector<std::string> parts_;
};

MsgBuilder txt(const std::string& text);
MsgBuilder at(uint64_t user_id);
MsgBuilder image(const std::string& path);

}} // namespace luo9::message

#endif
