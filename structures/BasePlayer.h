//
// Created by Parti on 2021/3/3.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::structures {
    class BasePlayer {
    public:
        std::shared_ptr<std::unordered_map<std::string, uint64_t>> getSidsMap() const;

    private:
        std::shared_ptr<std::unordered_map<std::string, uint64_t>> _sidsMap{};
    };
}
