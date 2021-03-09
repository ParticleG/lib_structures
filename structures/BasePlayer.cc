//
// Created by Parti on 2021/3/3.
//

#include <structures/BasePlayer.h>

using namespace std;
using namespace tech::structures;

BasePlayer::BasePlayer(const bool &singleSid) : _singleSid(singleSid) {}

unordered_map<string, uint64_t> BasePlayer::getSidsMap() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _sidsMap;
}

bool BasePlayer::isSingleSid() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _singleSid;
}

void BasePlayer::setSidsMap(std::unordered_map<std::string, uint64_t> &&sidsMap) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _sidsMap = move(sidsMap);
}

