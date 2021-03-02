//
// Created by Parti on 2021/2/23.
//

#include <structures/Chat.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

Chat::Chat(const int64_t &id) {
    Mapper<Techmino::Info> infoMapper(app().getDbClient());
    auto info = infoMapper.findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id));
    _info = make_shared<Techmino::Info>(info);
}

shared_ptr<Techmino::Info> Chat::getInfo() const {
    return _info;
}
