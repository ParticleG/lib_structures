//
// Created by Parti on 2021/2/19.
//

#include <structures/App.h>

using namespace tech::structures;
using namespace std;

App::App(const int &versionCode) : _versionCode(versionCode) {}

void App::setID(const int64_t id) {
    _id = id;
}

int64_t App::getID() const {
    return _id;
}

void App::setAuthToken(const string &authToken) {
    _authToken = authToken;
}

std::string App::getAuthToken() const {
    return _authToken;
}

void App::setAccessToken(const string &accessToken) {
    _accessToken = accessToken;
}

std::string App::getAccessToken() const {
    return _accessToken;
}
