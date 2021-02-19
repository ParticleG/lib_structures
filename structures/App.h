//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <models/Auth.h>

namespace tech::structures {
    class App {
    public:
        App(const int &versionCode);

        void setID(const int64_t id);

        int64_t getID() const;

        void setAuthToken(const std::string &authToken);

        std::string getAuthToken() const;

        void setAccessToken(const std::string &accessToken);

        std::string getAccessToken() const;

    private:
        int _versionCode;
        int64_t _id{};
        std::string _authToken{}, _accessToken{};
    };
}
