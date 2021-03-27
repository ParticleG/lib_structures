//
// Created by Parti on 2021/2/19.
//

#include <drogon/drogon.h>
#include <structures/App.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

App::App() {
    Mapper<Techmino::App> appMapper(app().getDbClient());
    auto app = appMapper.orderBy(Techmino::App::Cols::_version_code, SortOrder::DESC).limit(1).findAll()[0];
    _app = make_shared<Techmino::App>(app);
}

std::shared_ptr<drogon_model::Techmino::App> App::getApp() const {
    return _app;
}
