#ifndef __BUS_H__
#define __BUS_H__

#include <string>
#include <vector>
#include <functional>
#include "head.h"

extern "C" {
    LUO9_API int luo9_bus_init();
    LUO9_API int luo9_bus_subscribe(const char* topic);
    LUO9_API int luo9_bus_publish(const char* topic, const char* payload);
    LUO9_API char* luo9_bus_pop(const char* topic, int subscriber_id);
    LUO9_API char* luo9_bus_wait_pop(const char* topic, int subscriber_id);
    LUO9_API void luo9_bus_free_string(char* ptr);
}

class Topic {
public:
    /// 订阅 topic，返回 subscriber_id（< 0 表示失败）
    int subscribe() const;

    /// 发布消息到 topic
    bool publish(const std::string& payload) const;

    /// 非阻塞取消息（队列为空返回空字符串）
    std::string pop(int subscriber_id) const;

    /// 阻塞取消息（挂起线程直到有消息）
    std::string wait_pop(int subscriber_id) const;

private:
    friend class Bus;
    std::string name_;
    explicit Topic(const std::string& name) : name_(name) {}
};

class Bus {
public:
    /// 初始化总线
    static bool init();

    /// 获取 topic 引用
    static Topic topic(const std::string& name);
};

#endif // __BUS_H__
