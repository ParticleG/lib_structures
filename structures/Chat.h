//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <models/Info.h>
#include <structures/BasePlayer.h>

namespace tech::structures {
    class Chat : public BasePlayer {
    public:
        explicit Chat(const int64_t &id);

        std::shared_ptr<drogon_model::Techmino::Info> getInfo() const;

        Json::Value getPlayerInfo(const std::string &message) const;

        Json::Value getPlayerInfo() const;

    private:
        std::shared_ptr<drogon_model::Techmino::Info> _info{};
    };
}
