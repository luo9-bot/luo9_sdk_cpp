#include "payload.h"
#include <cstring>
#include <cstdlib>

namespace {

const char* skip_ws(const char* p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    return p;
}

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

uint64_t parse_number(const char*& p) {
    p = skip_ws(p);
    uint64_t val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        ++p;
    }
    return val;
}

int64_t parse_signed_number(const char*& p) {
    p = skip_ws(p);
    bool negative = false;
    if (*p == '-') {
        negative = true;
        ++p;
    }
    int64_t val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        ++p;
    }
    return negative ? -val : val;
}

bool parse_bool(const char*& p) {
    p = skip_ws(p);
    if (strncmp(p, "true", 4) == 0) { p += 4; return true; }
    if (strncmp(p, "false", 5) == 0) { p += 5; return false; }
    return false;
}

bool is_null(const char* p) {
    p = skip_ws(p);
    return strncmp(p, "null", 4) == 0;
}

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
        while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ') ++p;
    }
    return p;
}

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

using namespace luo9::payload;

NoticeType parse_notice_type(const std::string& s) {
    if (s == "friend_add") return NoticeType::FriendAdd;
    if (s == "friend_recall") return NoticeType::FriendRecall;
    if (s == "group_admin") return NoticeType::GroupAdmin;
    if (s == "group_ban") return NoticeType::GroupBan;
    if (s == "group_increase") return NoticeType::GroupIncrease;
    if (s == "group_decrease") return NoticeType::GroupDecrease;
    if (s == "group_card") return NoticeType::GroupCard;
    if (s == "group_recall") return NoticeType::GroupRecall;
    if (s == "group_upload") return NoticeType::GroupUpload;
    if (s == "group_title") return NoticeType::GroupTitle;
    if (s == "honor") return NoticeType::Honor;
    if (s == "essence") return NoticeType::Essence;
    if (s == "poke") return NoticeType::Poke;
    if (s == "lucky_king") return NoticeType::LuckyKing;
    if (s == "group_msg_emoji_like") return NoticeType::GroupMsgEmojiLike;
    if (s == "notify") return NoticeType::Notify;
    return NoticeType::Unknown;
}

HonorType parse_honor_type(const std::string& s) {
    if (s == "talkative") return HonorType::Talkative;
    if (s == "performer") return HonorType::Performer;
    if (s == "emotion") return HonorType::Emotion;
    return HonorType::Unknown;
}

RequestType parse_request_type(const std::string& s) {
    if (s == "friend") return RequestType::Friend;
    if (s == "group") return RequestType::Group;
    return RequestType::Unknown;
}

GroupRequestSubType parse_group_request_sub_type(const std::string& s) {
    if (s == "add") return GroupRequestSubType::Add;
    if (s == "invite") return GroupRequestSubType::Invite;
    return GroupRequestSubType::Unknown;
}

MessagePayload parse_message(const char* obj) {
    MessagePayload msg;
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
    if ((v = find_key(obj, "time"))) msg.time = parse_number(v);
    if ((v = find_key(obj, "self_id"))) msg.self_id = parse_number(v);
    if ((v = find_key(obj, "message_id"))) msg.message_id = parse_number(v);
    if ((v = find_key(obj, "message_seq"))) {
        if (is_null(v)) msg.message_seq = std::nullopt;
        else msg.message_seq = parse_number(v);
    }
    if ((v = find_key(obj, "real_id"))) {
        if (is_null(v)) msg.real_id = std::nullopt;
        else msg.real_id = parse_number(v);
    }
    if ((v = find_key(obj, "real_seq"))) {
        if (is_null(v)) msg.real_seq = std::nullopt;
        else msg.real_seq = parse_string(v);
    }
    if ((v = find_key(obj, "sub_type"))) msg.sub_type = parse_string(v);
    if ((v = find_key(obj, "font"))) msg.font = static_cast<uint32_t>(parse_number(v));
    if ((v = find_key(obj, "sender"))) {
        if (is_null(v)) {
            msg.sender = std::nullopt;
        } else {
            Sender s;
            // Skip '{' for nested object
            if (*v == '{') ++v;
            const char* sv;
            if ((sv = find_key(v, "user_id"))) s.user_id = parse_number(sv);
            if ((sv = find_key(v, "nickname"))) s.nickname = parse_string(sv);
            if ((sv = find_key(v, "card"))) s.card = parse_string(sv);
            if ((sv = find_key(v, "sex"))) s.sex = parse_string(sv);
            if ((sv = find_key(v, "age"))) s.age = static_cast<uint32_t>(parse_number(sv));
            if ((sv = find_key(v, "area"))) s.area = parse_string(sv);
            if ((sv = find_key(v, "level"))) s.level = parse_string(sv);
            if ((sv = find_key(v, "role"))) s.role = parse_string(sv);
            if ((sv = find_key(v, "title"))) s.title = parse_string(sv);
            msg.sender = s;
        }
    }
    if ((v = find_key(obj, "anonymous"))) {
        if (is_null(v)) {
            msg.anonymous = std::nullopt;
        } else {
            Anonymous a;
            // Skip '{' for nested object
            if (*v == '{') ++v;
            const char* sv;
            if ((sv = find_key(v, "id"))) a.id = parse_number(sv);
            if ((sv = find_key(v, "name"))) a.name = parse_string(sv);
            if ((sv = find_key(v, "flag"))) a.flag = parse_string(sv);
            msg.anonymous = a;
        }
    }
    if ((v = find_key(obj, "message_format"))) msg.message_format = parse_string(v);
    return msg;
}

MetaEventPayload parse_meta_event(const char* obj) {
    MetaEventPayload ev;
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
    if ((v = find_key(obj, "sub_type"))) ev.sub_type = parse_string(v);
    if ((v = find_key(obj, "self_id"))) ev.self_id = parse_number(v);
    if ((v = find_key(obj, "status"))) {
        if (is_null(v)) {
            ev.status = std::nullopt;
        } else {
            Status s;
            // Skip '{' for nested object
            if (*v == '{') ++v;
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
    NoticePayload n;
    const char* v;
    if ((v = find_key(obj, "notice_type"))) {
        n.notice_type = parse_notice_type(parse_string(v));
    }
    if ((v = find_key(obj, "sub_type"))) n.sub_type = parse_string(v);
    if ((v = find_key(obj, "user_id"))) n.user_id = parse_number(v);
    if ((v = find_key(obj, "group_id"))) {
        if (is_null(v)) n.group_id = std::nullopt;
        else n.group_id = parse_number(v);
    }
    if ((v = find_key(obj, "time"))) n.time = parse_number(v);
    if ((v = find_key(obj, "operator_id"))) {
        if (is_null(v)) n.operator_id = std::nullopt;
        else n.operator_id = parse_number(v);
    }
    if ((v = find_key(obj, "target_id"))) {
        if (is_null(v)) n.target_id = std::nullopt;
        else n.target_id = parse_number(v);
    }
    if ((v = find_key(obj, "message_id"))) {
        if (is_null(v)) n.message_id = std::nullopt;
        else n.message_id = parse_number(v);
    }
    if ((v = find_key(obj, "file"))) {
        if (is_null(v)) {
            n.file = std::nullopt;
        } else {
            FileInfo f;
            // Skip '{' for nested object
            if (*v == '{') ++v;
            const char* sv;
            if ((sv = find_key(v, "id"))) f.id = parse_string(sv);
            if ((sv = find_key(v, "name"))) f.name = parse_string(sv);
            if ((sv = find_key(v, "size"))) f.size = parse_number(sv);
            if ((sv = find_key(v, "busid"))) f.busid = parse_signed_number(sv);
            n.file = f;
        }
    }
    if ((v = find_key(obj, "duration"))) {
        if (is_null(v)) n.duration = std::nullopt;
        else n.duration = parse_number(v);
    }
    if ((v = find_key(obj, "card_new"))) {
        if (is_null(v)) n.card_new = std::nullopt;
        else n.card_new = parse_string(v);
    }
    if ((v = find_key(obj, "card_old"))) {
        if (is_null(v)) n.card_old = std::nullopt;
        else n.card_old = parse_string(v);
    }
    if ((v = find_key(obj, "honor_type"))) {
        if (is_null(v)) n.honor_type = std::nullopt;
        else n.honor_type = parse_honor_type(parse_string(v));
    }
    if ((v = find_key(obj, "title"))) {
        if (is_null(v)) n.title = std::nullopt;
        else n.title = parse_string(v);
    }
    if ((v = find_key(obj, "flag"))) {
        if (is_null(v)) n.flag = std::nullopt;
        else n.flag = parse_string(v);
    }
    if ((v = find_key(obj, "comment"))) {
        if (is_null(v)) n.comment = std::nullopt;
        else n.comment = parse_string(v);
    }
    return n;
}

RequestPayload parse_request(const char* obj) {
    RequestPayload r;
    const char* v;
    if ((v = find_key(obj, "request_type"))) {
        r.request_type = parse_request_type(parse_string(v));
    }
    if ((v = find_key(obj, "user_id"))) r.user_id = parse_number(v);
    if ((v = find_key(obj, "group_id"))) {
        if (is_null(v)) r.group_id = std::nullopt;
        else r.group_id = parse_number(v);
    }
    if ((v = find_key(obj, "comment"))) r.comment = parse_string(v);
    if ((v = find_key(obj, "flag"))) r.flag = parse_string(v);
    if ((v = find_key(obj, "sub_type"))) {
        r.sub_type = parse_group_request_sub_type(parse_string(v));
    }
    if ((v = find_key(obj, "time"))) r.time = parse_number(v);
    if ((v = find_key(obj, "self_id"))) r.self_id = parse_number(v);
    return r;
}

} // anonymous namespace

namespace luo9 { namespace payload {

BusPayload BusPayload::parse(const std::string& json) {
    BusPayload result;
    const char* p = json.c_str();
    p = skip_ws(p);
    if (*p != '{') return result;
    ++p;

    p = skip_ws(p);
    if (*p != '"') return result;
    ++p;

    std::string outer_key;
    while (*p && *p != '"') outer_key += *p++;
    if (*p == '"') ++p;
    p = skip_ws(p);
    if (*p == ':') ++p;
    p = skip_ws(p);

    // Now p should point to '{' of the inner object, skip it
    if (*p == '{') ++p;

    if (outer_key == "Message") {
        result.type = PayloadType::Message;
        result.message = parse_message(p);
    } else if (outer_key == "MetaEvent") {
        result.type = PayloadType::MetaEvent;
        result.meta_event = parse_meta_event(p);
    } else if (outer_key == "Notice") {
        result.type = PayloadType::Notice;
        result.notice = parse_notice(p);
    } else if (outer_key == "Request") {
        result.type = PayloadType::Request;
        result.request = parse_request(p);
    }

    return result;
}

}} // namespace luo9::payload
