#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

#include <string>
#include <cstdint>
#include <optional>

// ── 消息类型 ────────────────────────────────────────────────────

enum class MsgType {
    Private,
    Group,
    Other,
};

struct MessagePayload {
    MsgType message_type;
    uint64_t user_id;
    std::optional<uint64_t> group_id;
    std::string message;
};

// ── 元事件类型 ──────────────────────────────────────────────────

enum class MetaEventType {
    Lifecycle,
    Heartbeat,
    Unknown,
};

enum class SubType {
    Enable, Disable, Connect,
    Friend, GroupTemp, GroupSelf, Other, Normal, Notice,
    Set, Unset, Ban, LiftBan, Leave, Kick, KickMe, Approve,
    Poke, InputStatus, Title, ProfileLike, Add, Invite,
    None,
};

struct Status {
    bool good;
    bool online;
};

struct MetaEventPayload {
    std::optional<uint64_t> interval;
    MetaEventType meta_event_type;
    SubType sub_type;
    uint64_t self_id;
    std::optional<Status> status;
    uint64_t time;
};

// ── 通知类型 ────────────────────────────────────────────────────

enum class NoticeType {
    FriendAdd, FriendRecall,
    GroupAdmin, GroupBan, GroupIncrease, GroupDecrease,
    GroupCard, GroupRecall, GroupUpload,
    Essence, Notify,
    Unknown,
};

struct NoticePayload {
    NoticeType notice_type;
    SubType sub_type;
    std::string status;
    uint64_t user_id;
    std::optional<uint64_t> group_id;
    uint64_t time;
};

// ── 总线载荷 ────────────────────────────────────────────────────

enum class PayloadType {
    Message,
    MetaEvent,
    Notice,
    Unknown,
};

struct BusPayload {
    PayloadType type;

    MessagePayload message;
    MetaEventPayload meta_event;
    NoticePayload notice;

    /// 从 JSON 字符串解析总线载荷
    static BusPayload parse(const std::string& json);
};

#endif // __PAYLOAD_H__
