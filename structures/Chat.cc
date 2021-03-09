//
// Created by Parti on 2021/2/23.
//

#include <structures/Chat.h>
#include <utils/misc.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Chat::Chat(const int64_t &id): BasePlayer(false) {
    Mapper<Techmino::Info> infoMapper(app().getDbClient());
    auto info = infoMapper.findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id));
    _info = make_shared<Techmino::Info>(info);
}

shared_ptr<Techmino::Info> Chat::getInfo() const {
    return _info;
}

Json::Value Chat::getPlayerInfo(const string &message) const {
    auto info = getInfo();
    Json::Value result;
    result["uid"] = info->getValueOfId();
    result["username"] = info->getValueOfId();
    result["time"] = misc::fromDate();
    result["message"] = message;
    return result;
}

Json::Value Chat::getPlayerInfo() const {
    auto info = getInfo();
    Json::Value result;
    result["uid"] = info->getValueOfId();
    result["username"] = info->getValueOfId();
    return result;
}
