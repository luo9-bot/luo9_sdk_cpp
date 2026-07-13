#include "version.h"
#include "bus.h"

namespace luo9 { namespace version {

bool is_version_query(const std::string& json) {
    size_t pos = json.find("\"action\"");
    if (pos == std::string::npos) return false;
    pos = json.find(":", pos);
    if (pos == std::string::npos) return false;
    pos = json.find("\"query\"", pos);
    return pos != std::string::npos;
}

void reply_version(const std::string& name, const std::string& ver) {
    std::string json = "{\"action\":\"response\",\"name\":\"" + name + "\",\"version\":\"" + ver + "\"}";
    bus::topic(TOPIC_VERSION_REPLY).publish(json);
}

}} // namespace luo9::version
