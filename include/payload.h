#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

#include <string>
#include <cstdint>
#include <optional>

namespace luo9 { namespace payload {

enum class MsgType { Private, Group, Other };

struct Sender {
    uint64_t user_id;
    std::string nickname;
    std::string card;
    std::string sex;
    uint32_t age;
    std::string area;
    std::string level;
    std::string role;
    std::string title;
    Sender() : user_id(0), age(0) {}
};

struct Anonymous {
    uint64_t id;
    std::string name;
    std::string flag;
    Anonymous() : id(0) {}
};

struct MessagePayload {
    MsgType message_type;
    uint64_t user_id;
    std::optional<uint64_t> group_id;
    std::string message;
    uint64_t time;
    uint64_t self_id;
    uint64_t message_id;
    std::optional<uint64_t> message_seq;
    std::optional<uint64_t> real_id;
    std::optional<std::string> real_seq;
    std::string sub_type;
    uint32_t font;
    std::optional<Sender> sender;
    std::optional<Anonymous> anonymous;
    std::string message_format;
    MessagePayload() : message_type(MsgType::Other), user_id(0), time(0), self_id(0), message_id(0), font(0) {}
};

enum class MetaEventType { Lifecycle, Heartbeat, Unknown };

struct Status {
    bool good;
    bool online;
    Status() : good(false), online(false) {}
};

struct MetaEventPayload {
    std::optional<uint64_t> interval;
    MetaEventType meta_event_type;
    std::string sub_type;
    uint64_t self_id;
    std::optional<Status> status;
    uint64_t time;
    MetaEventPayload() : meta_event_type(MetaEventType::Unknown), self_id(0), time(0) {}
};

enum class NoticeType {
    FriendAdd, FriendRecall,
    GroupAdmin, GroupBan, GroupIncrease, GroupDecrease,
    GroupCard, GroupRecall, GroupUpload,
    GroupTitle, Honor, Essence, Poke, LuckyKing, GroupMsgEmojiLike,
    Notify,
    Unknown,
};

struct FileInfo {
    std::string id;
    std::string name;
    uint64_t size;
    int64_t busid;
    FileInfo() : size(0), busid(0) {}
};

enum class HonorType { Talkative, Performer, Emotion, Unknown };

struct NoticePayload {
    NoticeType notice_type;
    std::string sub_type;
    uint64_t user_id;
    std::optional<uint64_t> group_id;
    uint64_t time;
    std::optional<uint64_t> operator_id;
    std::optional<uint64_t> target_id;
    std::optional<uint64_t> message_id;
    std::optional<FileInfo> file;
    std::optional<uint64_t> duration;
    std::optional<std::string> card_new;
    std::optional<std::string> card_old;
    std::optional<HonorType> honor_type;
    std::optional<std::string> title;
    std::optional<std::string> flag;
    std::optional<std::string> comment;
    NoticePayload() : notice_type(NoticeType::Unknown), user_id(0), time(0) {}
};

enum class RequestType { Friend, Group, Unknown };
enum class GroupRequestSubType { Add, Invite, Unknown };

struct RequestPayload {
    RequestType request_type;
    uint64_t user_id;
    std::optional<uint64_t> group_id;
    std::string comment;
    std::string flag;
    GroupRequestSubType sub_type;
    uint64_t time;
    uint64_t self_id;
    RequestPayload() : request_type(RequestType::Unknown), user_id(0), sub_type(GroupRequestSubType::Unknown), time(0), self_id(0) {}
};

enum class PayloadType { Message, MetaEvent, Notice, Request, Unknown };

struct BusPayload {
    PayloadType type;
    MessagePayload message;
    MetaEventPayload meta_event;
    NoticePayload notice;
    RequestPayload request;
    BusPayload() : type(PayloadType::Unknown) {}
    static BusPayload parse(const std::string& json);
};

}} // namespace luo9::payload

#endif
