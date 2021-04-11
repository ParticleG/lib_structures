//
// Created by Parti on 2021/3/3.
//

#include <structures/BasePlayer.h>

using namespace std;
using namespace tech::structures;

BasePlayer::BasePlayer(const bool &singleSid) {
    if (singleSid) {
        _sids.emplace<0>(std::pair<std::string, uint64_t>());
    } else {
        _sids.emplace<1>(std::unordered_map<std::string, uint64_t>());
    }
}

bool BasePlayer::isSingleSid() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _sids.index() == 0;
}

variant<string, vector<string>> BasePlayer::getRid() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    if (auto sidsPtr = get_if<0>(&_sids)) {
        return sidsPtr->first;
    } else {
        vector<string> result;
        for (const auto &pair : get<1>(_sids)) {
            result.push_back(pair.first);
        }
        return result;
    }
}

uint64_t BasePlayer::getSid(const std::string &rid) const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    if (auto sidsPtr = get_if<0>(&_sids)) {
        return sidsPtr->second;
    } else {
        return get<1>(_sids).at(rid);
    }
}

void BasePlayer::setSid(const string &rid) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    if (auto sidsPtr = get_if<0>(&_sids)) {
        *sidsPtr = std::pair<std::string, uint64_t>();
    } else {
        get<1>(_sids).erase(rid);
    }
}

void BasePlayer::setSid(const string &rid, const uint64_t &sid) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    if (auto sidsPtr = get_if<0>(&_sids)) {
        *sidsPtr = make_pair(rid, sid);
    } else {
        get<1>(_sids).emplace(make_pair(rid, sid));
    }
}
