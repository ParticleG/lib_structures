//
// Created by Parti on 2021/3/3.
//

#pragma once

#include <drogon/drogon.h>
#include <shared_mutex>

namespace tech::structures {
    class BasePlayer {
    public:
        BasePlayer() = delete;

        explicit BasePlayer(const bool &singleSid);

        bool isSingleSid() const;

        std::unordered_map<std::string, uint64_t> getSidsMap() const;

        void setSidsMap(std::unordered_map<std::string, uint64_t> &&sidsMap);

        virtual ~BasePlayer() noexcept {};

    private:
        const bool _singleSid;
        std::unordered_map<std::string, uint64_t> _sidsMap{}; // Room - SID
        mutable std::shared_mutex _sharedMutex;
    };
}
