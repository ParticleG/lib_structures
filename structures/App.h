//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <models/App.h>
#include <models/Message.h>

namespace tech::structures {
    class App {
    public:
        App();

        [[nodiscard]] std::shared_ptr<drogon_model::Techmino::App> getApp() const;

    private:
        std::shared_ptr<drogon_model::Techmino::App> _app{};
    };
}
