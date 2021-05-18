//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <structures/BasePlayer.h>

namespace tech::structures {
    class Stream : public BasePlayer {
    public:
        explicit Stream(const int &uid);

        const int &getUid() const;

        void addHistory(const std::string &message);

        Json::Value parseHistory() const;

        uint64_t getScore() const;

        void setScore(const uint64_t &score);

        uint64_t getSurvivalTime() const;

        void setSurvivalTime(const uint64_t &survivalTime);

        uint64_t getPlace() const;

        void setPlace(const uint64_t &place);

        bool getSpectate() const;

        void setSpectate(const bool &spectate);

        bool getDead() const;

        void setDead(const bool &dead);

        Json::Value parsePlayerInfo(Json::Value &&data) const;

        ~Stream() override;

    private:
        const int _uid;
        std::string _history;
        std::atomic<uint64_t> _score{}, _survivalTime{}, _place{};
        std::atomic<bool> _spectate{}, _dead{};
    };
}
