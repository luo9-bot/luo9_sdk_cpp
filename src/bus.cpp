#include "bus.h"

// ── Topic ───────────────────────────────────────────────────────

int Topic::subscribe() const {
    return luo9_bus_subscribe(name_.c_str());
}

bool Topic::publish(const std::string& payload) const {
    return luo9_bus_publish(name_.c_str(), payload.c_str()) == 0;
}

std::string Topic::pop(int subscriber_id) const {
    char* ptr = luo9_bus_pop(name_.c_str(), subscriber_id);
    if (!ptr) return "";
    std::string result(ptr);
    luo9_bus_free_string(ptr);
    return result;
}

std::string Topic::wait_pop(int subscriber_id) const {
    char* ptr = luo9_bus_wait_pop(name_.c_str(), subscriber_id);
    if (!ptr) return "";
    std::string result(ptr);
    luo9_bus_free_string(ptr);
    return result;
}

// ── Bus ─────────────────────────────────────────────────────────

bool Bus::init() {
    return luo9_bus_init() == 0;
}

Topic Bus::topic(const std::string& name) {
    return Topic(name);
}
