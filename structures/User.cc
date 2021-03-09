//
// Created by Parti on 2021/2/23.
//

#include <drogon/drogon.h>
#include "structures/User.h"

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

User::User(const int64_t &id) {
    Mapper<Techmino::Auth> authMapper(app().getDbClient());
    Mapper<Techmino::Info> infoMapper(app().getDbClient());
    auto auth = authMapper.findOne(Criteria(Techmino::Auth::Cols::__id, CompareOperator::EQ, id));
    auto info = infoMapper.findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id));
    _auth = make_shared<Techmino::Auth>(auth);
    _info = make_shared<Techmino::Info>(info);
}

shared_ptr<Techmino::Auth> User::getAuth() const {
    return _auth;
}

shared_ptr<Techmino::Info> User::getInfo() const {
    return _info;
}

