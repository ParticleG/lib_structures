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

Chat::Chat(const int64_t &id) :
        BasePlayer(false),
        _info(Mapper<Techmino::Info>(app().getDbClient()).findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id))) {}

const Techmino::Info &Chat::getInfo() const {
    return _info;
}

Json::Value Chat::getPlayerInfo(const string &message) const {
    Json::Value result;
    result["uid"] = _info.getValueOfId();
    result["username"] = _info.getValueOfId();
    result["time"] = misc::fromDate();
    result["message"] = message;
    return result;
}

Json::Value Chat::getPlayerInfo() const {
    Json::Value result;
    result["uid"] = _info.getValueOfId();
    result["username"] = _info.getValueOfId();
    return result;
}
