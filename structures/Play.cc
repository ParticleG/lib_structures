//
// Created by Parti on 2021/2/23.
//

#include <structures/Play.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

Play::Play(const int64_t &id) :
        BasePlayer(true),
        _config(string()),
        _mode(PlayMode::standby),
        _info(Mapper<Techmino::Info>(app().getDbClient()).findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id))) {}


const Techmino::Info &Play::getInfo() const {
    return _info;
}

string Play::getConfig() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _config;
}

void Play::setConfig(string &&config) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _config = move(config);
}

Play::PlayMode Play::getMode() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _mode;
}

void Play::setMode(const PlayMode mode) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _mode = mode;
}

void Play::setMode(const int64_t mode) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _mode = static_cast<PlayMode>(mode);
}

Json::Value Play::parsePlayerInfo(Json::Value &&data) const {
    data["sid"] = getSid();
    data["uid"] = _info.getValueOfId();
    data["username"] = _info.getValueOfUsername();
    return data;
}
