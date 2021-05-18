//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <models/Info.h>
#include <structures/BasePlayer.h>

namespace tech::structures {
    class Play : public BasePlayer {
    public:
        enum class PlayMode {
            standby,
            ready,
            spectate
        };

        explicit Play(const int64_t &id);

        const drogon_model::Techmino::Info &getInfo() const;

        std::string getConfig() const;

        void setConfig(std::string &&config);

        PlayMode getMode() const;

        void setMode(PlayMode mode);

        void setMode(int64_t mode);

        Json::Value parsePlayerInfo(Json::Value &&data) const;

    private:
        mutable std::shared_mutex _sharedMutex;
        drogon_model::Techmino::Info _info{};
        std::string _config;
        PlayMode _mode;
    };
}
