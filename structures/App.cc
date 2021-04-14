//
// Created by Parti on 2021/2/19.
//

#include <drogon/drogon.h>
#include <structures/App.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::structures;
using namespace std;

App::App() : _app(
        Mapper<Techmino::App>(app().getDbClient())
                .orderBy(Techmino::App::Cols::_version_code, SortOrder::DESC)
                .limit(1)
                .findAll()[0]
) {}

const Techmino::App &App::getApp() const {
    return _app;
}
