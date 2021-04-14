//
// Created by Parti on 2021/2/23.
//

#include <drogon/drogon.h>
#include <structures/User.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

User::User(const int64_t &id) :
        _info(Mapper<Techmino::Info>(app().getDbClient()).findOne(Criteria(Techmino::Info::Cols::__id, CompareOperator::EQ, id))),
        _auth(Mapper<Techmino::Auth>(app().getDbClient()).findOne(Criteria(Techmino::Auth::Cols::__id, CompareOperator::EQ, id))) {}

const Techmino::Auth &User::getAuth() const { return _auth; }

const Techmino::Info &User::getInfo() const { return _info; }
