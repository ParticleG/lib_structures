//
// Created by Parti on 2021/3/3.
//

#include "structures/BasePlayer.h"

using namespace std;
using namespace tech::structures;

BasePlayer::BasePlayer(const bool &singleSid) : _singleSid(singleSid) {}

shared_ptr<unordered_map<string, uint64_t>> BasePlayer::getSidsMap() const {
    return _sidsMap;
}

bool BasePlayer::isSingleSid() const {
    return _singleSid;
}

