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
        _ready(false),
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

bool Play::getReady() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _ready;
}

void Play::setReady(const bool ready) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _ready = ready;
}

Json::Value Play::parsePlayerInfo(Json::Value &&data) const {
    data["sid"] = getSid();
    data["uid"] = _info.getValueOfId();
    data["username"] = _info.getValueOfUsername();
    return data;
}
