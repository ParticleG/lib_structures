//
// Created by Parti on 2021/2/23.
//

#include <structures/Play.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

Play::Play(const int64_t &id) : BasePlayer(true) {
    Mapper<Techmino::Info> infoMapper(app().getDbClient());
    auto info = infoMapper.findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id));
    _info = make_shared<Techmino::Info>(info);
}

shared_ptr<Techmino::Info> Play::getInfo() const {
    return _info;
}

string Play::getConfig() const {
    return _config;
}

void Play::setConfig(string &&config) {
    _config = move(config);
}

bool Play::getReady() const {
    return _ready;
}

void Play::setReady(const bool ready) {
    _ready = ready;
}

Json::Value Play::parsePlayerInfo(Json::Value &&data) const {
    auto info = getInfo();
    data["sid"] = getSidsMap().begin()->second;
    data["uid"] = info->getValueOfId();
    data["username"] = info->getValueOfId();
    return data;
}
