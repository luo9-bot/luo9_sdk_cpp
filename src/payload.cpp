#include "payload.h"
#include <cstring>
#include <cstdlib>
#include <algorithm>

// ── 最小化 JSON 解析工具 ────────────────────────────────────────

namespace {

// 跳过空白
const char* skip_ws(const char* p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    return p;
}

// 匹配字面量
const char* expect(const char* p, const char* lit) {
    p = skip_ws(p);
    size_t len = strlen(lit);
    if (strncmp(p, lit, len) == 0) return p + len;
    return nullptr;
}

// 解析 JSON 字符串值（不含外层引号）
std::string parse_string(const char*& p) {
    p = skip_ws(p);
    if (*p != '"') return "";
    ++p;
    std::string result;
    while (*p && *p != '"') {
        if (*p == '\\') {
            ++p;
            if (*p) result += *p++;
        } else {
            result += *p++;
        }
    }
    if (*p == '"') ++p;
    return result;
}

// 解析 JSON 数值
uint64_t parse_number(const char*& p) {
    p = skip_ws(p);
    uint64_t val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        ++p;
    }
    return val;
}

// 解析 JSON 布尔
bool parse_bool(const char*& p) {
    p = skip_ws(p);
    if (strncmp(p, "true", 4) == 0) { p += 4; return true; }
    if (strncmp(p, "false", 5) == 0) { p += 5; return false; }
    return false;
}

// 检查是否为 null
bool is_null(const char* p) {
    p = skip_ws(p);
    return strncmp(p, "null", 4) == 0;
}

// 跳过一个 JSON 值（字符串、数字、对象、数组、true/false/null）
const char* skip_value(const char* p) {
    p = skip_ws(p);
    if (*p == '"') {
        ++p;
        while (*p && *p != '"') {
            if (*p == '\\') ++p;
            ++p;
        }
        if (*p == '"') ++p;
    } else if (*p == '{') {
        int depth = 1;
        ++p;
        while (*p && depth > 0) {
            if (*p == '"') {
                ++p;
                while (*p && *p != '"') { if (*p == '\\') ++p; ++p; }
                if (*p == '"') ++p;
            } else {
                if (*p == '{') ++depth;
                if (*p == '}') --depth;
                ++p;
            }
        }
    } else if (*p == '[') {
        int depth = 1;
        ++p;
        while (*p && depth > 0) {
            if (*p == '"') {
                ++p;
                while (*p && *p != '"') { if (*p == '\\') ++p; ++p; }
                if (*p == '"') ++p;
            } else {
                if (*p == '[') ++depth;
                if (*p == ']') --depth;
                ++p;
            }
        }
    } else {
        // number / true / false / null
        while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ') ++p;
    }
    return p;
}

// 在 JSON 对象中查找指定 key 的值位置
const char* find_key(const char* json, const char* key) {
    size_t key_len = strlen(key);
    const char* p = json;
    while (*p) {
        p = skip_ws(p);
        if (*p == '"') {
            ++p;
            const char* key_start = p;
            while (*p && *p != '"') ++p;
            size_t len = p - key_start;
            if (*p == '"') ++p;
            p = skip_ws(p);
            if (*p == ':') ++p;
            if (len == key_len && strncmp(key_start, key, key_len) == 0) {
                return skip_ws(p);
            }
            p = skip_value(p);
            p = skip_ws(p);
            if (*p == ',') ++p;
        } else {
            break;
        }
    }
    return nullptr;
}

// 解析子类型字符串
SubType parse_sub_type(const std::string& s) {
    if (s == "enable") return SubType::Enable;
    if (s == "disable") return SubType::Disable;
    if (s == "connect") return SubType::Connect;
    if (s == "friend") return SubType::Friend;
    if (s == "group") return SubType::GroupTemp;
    if (s == "group_self") return SubType::GroupSelf;
    if (s == "other") return SubType::Other;
    if (s == "normal") return SubType::Normal;
    if (s == "notice") return SubType::Notice;
    if (s == "set") return SubType::Set;
    if (s == "unset") return SubType::Unset;
    if (s == "ban") return SubType::Ban;
    if (s == "lift_ban") return SubType::LiftBan;
    if (s == "leave") return SubType::Leave;
    if (s == "kick") return SubType::Kick;
    if (s == "kick_me") return SubType::KickMe;
    if (s == "approve") return SubType::Approve;
    if (s == "poke") return SubType::Poke;
    if (s == "input_status") return SubType::InputStatus;
    if (s == "title") return SubType::Title;
    if (s == "profile_like") return SubType::ProfileLike;
    if (s == "add") return SubType::Add;
    if (s == "invite") return SubType::Invite;
    return SubType::None;
}

// ── 解析各类型 ──────────────────────────────────────────────────

MessagePayload parse_message(const char* obj) {
    MessagePayload msg{};
    msg.message_type = MsgType::Other;

    const char* v;
    if ((v = find_key(obj, "message_type"))) {
        std::string s = parse_string(v);
        if (s == "private") msg.message_type = MsgType::Private;
        else if (s == "group") msg.message_type = MsgType::Group;
    }
    if ((v = find_key(obj, "user_id"))) msg.user_id = parse_number(v);
    if ((v = find_key(obj, "group_id"))) {
        if (is_null(v)) msg.group_id = std::nullopt;
        else msg.group_id = parse_number(v);
    }
    if ((v = find_key(obj, "message"))) msg.message = parse_string(v);

    return msg;
}

MetaEventPayload parse_meta_event(const char* obj) {
    MetaEventPayload ev{};
    ev.meta_event_type = MetaEventType::Unknown;
    ev.sub_type = SubType::None;

    const char* v;
    if ((v = find_key(obj, "meta_event_type"))) {
        std::string s = parse_string(v);
        if (s == "lifecycle") ev.meta_event_type = MetaEventType::Lifecycle;
        else if (s == "heartbeat") ev.meta_event_type = MetaEventType::Heartbeat;
    }
    if ((v = find_key(obj, "interval"))) {
        if (is_null(v)) ev.interval = std::nullopt;
        else ev.interval = parse_number(v);
    }
    if ((v = find_key(obj, "sub_type"))) ev.sub_type = parse_sub_type(parse_string(v));
    if ((v = find_key(obj, "self_id"))) ev.self_id = parse_number(v);
    if ((v = find_key(obj, "status"))) {
        if (is_null(v)) {
            ev.status = std::nullopt;
        } else {
            Status s{};
            const char* sv;
            if ((sv = find_key(v, "good"))) s.good = parse_bool(sv);
            if ((sv = find_key(v, "online"))) s.online = parse_bool(sv);
            ev.status = s;
        }
    }
    if ((v = find_key(obj, "time"))) ev.time = parse_number(v);

    return ev;
}

NoticePayload parse_notice(const char* obj) {
    NoticePayload n{};
    n.notice_type = NoticeType::Unknown;
    n.sub_type = SubType::None;

    const char* v;
    if ((v = find_key(obj, "notice_type"))) {
        std::string s = parse_string(v);
        if (s == "friend_add") n.notice_type = NoticeType::FriendAdd;
        else if (s == "friend_recall") n.notice_type = NoticeType::FriendRecall;
        else if (s == "group_admin") n.notice_type = NoticeType::GroupAdmin;
        else if (s == "group_ban") n.notice_type = NoticeType::GroupBan;
        else if (s == "group_increase") n.notice_type = NoticeType::GroupIncrease;
        else if (s == "group_decrease") n.notice_type = NoticeType::GroupDecrease;
        else if (s == "group_card") n.notice_type = NoticeType::GroupCard;
        else if (s == "group_recall") n.notice_type = NoticeType::GroupRecall;
        else if (s == "group_upload") n.notice_type = NoticeType::GroupUpload;
        else if (s == "essence") n.notice_type = NoticeType::Essence;
        else if (s == "notify") n.notice_type = NoticeType::Notify;
    }
    if ((v = find_key(obj, "sub_type"))) n.sub_type = parse_sub_type(parse_string(v));
    if ((v = find_key(obj, "status"))) n.status = parse_string(v);
    if ((v = find_key(obj, "user_id"))) n.user_id = parse_number(v);
    if ((v = find_key(obj, "group_id"))) {
        if (is_null(v)) n.group_id = std::nullopt;
        else n.group_id = parse_number(v);
    }
    if ((v = find_key(obj, "time"))) n.time = parse_number(v);

    return n;
}

} // anonymous namespace

// ── BusPayload::parse ───────────────────────────────────────────

BusPayload BusPayload::parse(const std::string& json) {
    BusPayload result{};
    result.type = PayloadType::Unknown;

    const char* p = json.c_str();
    p = skip_ws(p);
    if (*p != '{') return result;
    ++p;

    // 外层只有一个 key: "Message" / "MetaEvent" / "Notice"
    p = skip_ws(p);
    if (*p != '"') return result;
    ++p;

    std::string outer_key;
    while (*p && *p != '"') outer_key += *p++;
    if (*p == '"') ++p;
    p = skip_ws(p);
    if (*p == ':') ++p;
    p = skip_ws(p);

    if (outer_key == "Message") {
        result.type = PayloadType::Message;
        result.message = parse_message(p);
    } else if (outer_key == "MetaEvent") {
        result.type = PayloadType::MetaEvent;
        result.meta_event = parse_meta_event(p);
    } else if (outer_key == "Notice") {
        result.type = PayloadType::Notice;
        result.notice = parse_notice(p);
    }

    return result;
}
