#include "message.h"

namespace luo9 { namespace message {

MsgBuilder& MsgBuilder::txt(const std::string& text) {
    parts_.push_back(text);
    return *this;
}

MsgBuilder& MsgBuilder::at(uint64_t user_id) {
    parts_.push_back("[CQ:at,qq=" + std::to_string(user_id) + "]");
    return *this;
}

MsgBuilder& MsgBuilder::image(const std::string& path) {
    if (path.find("http") != std::string::npos) {
        parts_.push_back("[CQ:image,url=" + path + "]");
    } else {
        parts_.push_back("[CQ:image,file=" + path + "]");
    }
    return *this;
}

MsgBuilder& MsgBuilder::endl() {
    parts_.push_back("\n");
    return *this;
}

std::string MsgBuilder::build() const {
    std::string result;
    for (const auto& part : parts_) {
        result += part;
    }
    return result;
}

MsgBuilder txt(const std::string& text) {
    return MsgBuilder().txt(text);
}

MsgBuilder at(uint64_t user_id) {
    return MsgBuilder().at(user_id);
}

MsgBuilder image(const std::string& path) {
    return MsgBuilder().image(path);
}

}} // namespace luo9::message
