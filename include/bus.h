#ifndef __BUS_H__
#define __BUS_H__

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

namespace luo9 { namespace bus {

struct PluginSubscribers {
    int message_sub_id;
    int meta_event_sub_id;
    int notice_sub_id;
    int request_sub_id;
    int task_sub_id;
    int send_sub_id;
};

const char SENTINEL[] = "__luo9_unsubscribed__";

class Topic {
public:
    int subscribe() const;
    bool unsubscribe(int subscriber_id) const;
    bool publish(const std::string& payload) const;
    bool publish_to(const std::string& payload, const std::vector<int>& subscriber_ids) const;
    std::string pop(int subscriber_id) const;
    std::string wait_pop(int subscriber_id) const;

private:
    friend Topic topic(const std::string&);
    std::string name_;
    explicit Topic(const std::string& name);
};

bool init();
Topic topic(const std::string& name);
void init_subscribers(const PluginSubscribers* subscribers);

}} // namespace luo9::bus

#endif
