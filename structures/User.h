//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <models/Auth.h>
#include <models/Info.h>

namespace tech::structures {
    class User {
    public:
        User() = default;

        explicit User(const int64_t &id);

        [[nodiscard]] std::shared_ptr<drogon_model::Techmino::Auth> getAuth() const;

        [[nodiscard]] std::shared_ptr<drogon_model::Techmino::Info> getInfo() const;

    private:
        std::shared_ptr<drogon_model::Techmino::Auth> _auth{};
        std::shared_ptr<drogon_model::Techmino::Info> _info{};
    };
}
