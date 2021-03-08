//
// Created by Parti on 2021/2/23.
//

#pragma once

#include "models/Info.h"
#include "structures/BasePlayer.h"

namespace tech::structures {
    class Play : public BasePlayer {
    public:
        explicit Play(const int64_t &id);

        std::shared_ptr<drogon_model::Techmino::Info> getInfo() const;

        std::string getConfig() const;

        void setConfig(std::string &&config);

        bool getReady() const;

        void setReady(const bool ready);

    private:
        std::shared_ptr<drogon_model::Techmino::Info> _info{};
        std::string _config{};
        bool _ready{};
    };
}
