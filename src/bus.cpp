#include "bus.h"
#include <mutex>

extern "C" {
    int luo9_bus_init();
    int luo9_bus_subscribe(const char* topic);
    int luo9_bus_unsubscribe(const char* topic, int subscriber_id);
    int luo9_bus_publish(const char* topic, const char* payload);
    int luo9_bus_publish_to(const char* topic, const char* payload,
                            const int* subscriber_ids_ptr, int subscriber_ids_len);
    char* luo9_bus_pop(const char* topic, int subscriber_id);
    char* luo9_bus_wait_pop(const char* topic, int subscriber_id);
    void luo9_bus_free_string(char* ptr);
}

namespace luo9 { namespace bus {

namespace {
    std::unordered_map<std::string, int> g_precreated_subscribers;
    std::mutex g_mutex;
}

Topic::Topic(const std::string& name) : name_(name) {}

int Topic::subscribe() const {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_precreated_subscribers.find(name_);
        if (it != g_precreated_subscribers.end()) {
            return it->second;
        }
    }
    return luo9_bus_subscribe(name_.c_str());
}

bool Topic::unsubscribe(int subscriber_id) const {
    return luo9_bus_unsubscribe(name_.c_str(), subscriber_id) == 0;
}

bool Topic::publish(const std::string& payload) const {
    return luo9_bus_publish(name_.c_str(), payload.c_str()) == 0;
}

bool Topic::publish_to(const std::string& payload, const std::vector<int>& subscriber_ids) const {
    return luo9_bus_publish_to(name_.c_str(), payload.c_str(),
                               subscriber_ids.data(), static_cast<int>(subscriber_ids.size())) == 0;
}

std::string Topic::pop(int subscriber_id) const {
    char* ptr = luo9_bus_pop(name_.c_str(), subscriber_id);
    if (!ptr) return "";
    std::string result(ptr);
    luo9_bus_free_string(ptr);
    if (result == SENTINEL) return "";
    return result;
}

std::string Topic::wait_pop(int subscriber_id) const {
    char* ptr = luo9_bus_wait_pop(name_.c_str(), subscriber_id);
    if (!ptr) return "";
    std::string result(ptr);
    luo9_bus_free_string(ptr);
    if (result == SENTINEL) return "";
    return result;
}

bool init() {
    return luo9_bus_init() == 0;
}

Topic topic(const std::string& name) {
    return Topic(name);
}

void init_subscribers(const PluginSubscribers* subscribers) {
    if (!subscribers) return;
    std::lock_guard<std::mutex> lock(g_mutex);
    g_precreated_subscribers.clear();
    if (subscribers->message_sub_id >= 0)
        g_precreated_subscribers["luo9_message"] = subscribers->message_sub_id;
    if (subscribers->meta_event_sub_id >= 0)
        g_precreated_subscribers["luo9_meta_event"] = subscribers->meta_event_sub_id;
    if (subscribers->notice_sub_id >= 0)
        g_precreated_subscribers["luo9_notice"] = subscribers->notice_sub_id;
    if (subscribers->request_sub_id >= 0)
        g_precreated_subscribers["luo9_request"] = subscribers->request_sub_id;
    if (subscribers->task_sub_id >= 0)
        g_precreated_subscribers["luo9_task"] = subscribers->task_sub_id;
    if (subscribers->send_sub_id >= 0)
        g_precreated_subscribers["luo9_send"] = subscribers->send_sub_id;
}

}} // namespace luo9::bus

// Export luo9_init_subscribers for host to call
extern "C" void luo9_init_subscribers(const luo9::bus::PluginSubscribers* subscribers) {
    luo9::bus::init_subscribers(subscribers);
}
