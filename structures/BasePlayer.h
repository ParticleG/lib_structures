//
// Created by Parti on 2021/3/3.
//

#pragma once

#include <drogon/drogon.h>
#include <shared_mutex>
#include <variant>

namespace tech::structures {
    class BasePlayer {
    public:
        BasePlayer() = delete;

        explicit BasePlayer(const bool &singleSid);

        bool isSingleSid() const;

        std::variant<std::string, std::vector<std::string>> getRid() const;

        uint64_t getSid(const std::string &rid = "") const;

        void setSid(const std::string &rid);

        void setSid(const std::string &rid, const uint64_t &sid);

        virtual ~BasePlayer() noexcept = default;

    private:
        std::variant<std::pair<std::string, uint64_t>, std::unordered_map<std::string, uint64_t>> _sids;
        mutable std::shared_mutex _sharedMutex;
    };
}
