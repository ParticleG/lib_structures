//
// Created by Parti on 2021/2/19.
//

#include <drogon/drogon.h>
#include <structures/App.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

App::App(const int &versionCode){
    Mapper<Techmino::App> authMapper(app().getDbClient());
    auto app = authMapper.findOne(Criteria(Techmino::App::Cols::_version_code, CompareOperator::EQ, versionCode));
    _app = make_shared<Techmino::App>(app);
}

std::shared_ptr<drogon_model::Techmino::App> App::getApp() const {
    return _app;
}
