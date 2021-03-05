//
// Created by Parti on 2021/3/3.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::structures {
    class BasePlayer {
    public:
        BasePlayer() = delete;

        explicit BasePlayer(const bool &singleSid);

        bool isSingleSid() const;

        std::shared_ptr<std::unordered_map<std::string, uint64_t>> getSidsMap() const;

    private:
        const bool _singleSid;
        std::shared_ptr<std::unordered_map<std::string, uint64_t>> _sidsMap{};
    };
}
