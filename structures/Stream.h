//
// Created by Parti on 2021/2/23.
//

#pragma once

#include "structures/BasePlayer.h"

namespace tech::structures {
    class Stream : public BasePlayer {
    public:
        explicit Stream(const int &uid);

        int getUid() const;

        uint64_t getScore() const;

        void setScore(const uint64_t &score);

        uint64_t getSurvivalTime() const;

        void setSurvivalTime(const uint64_t &survivalTime);

        uint64_t getPlace() const;

        void setPlace(const uint64_t &place);

        bool getDead() const;

        void setDead(const bool &dead);

    private:
        const int _uid;
        uint64_t _score{}, _survivalTime{}, _place{};
        bool _dead{};
    };
}
