#ifndef __VERSION_H__
#define __VERSION_H__

#include <string>

namespace luo9 { namespace version {

const char TOPIC_VERSION[] = "luo9_version";
const char TOPIC_VERSION_REPLY[] = "luo9_version_reply";

bool is_version_query(const std::string& json);
void reply_version(const std::string& name, const std::string& version);

}} // namespace luo9::version

#endif
