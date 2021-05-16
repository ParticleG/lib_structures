//
// Created by Parti on 2021/2/27.
//

#include <structures/PlayRoom.h>
#include <utils/crypto.h>
#include <utils/misc.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

PlayRoom::PlayRoom(
        string id,
        const string &password,
        const uint64_t &capacity,
        Json::Value info,
        Json::Value data
) : BaseRoom(move(id), capacity),
    _encryptedPassword(crypto::blake2b(password, 1)),
    _info(move(info)),
    _data(move(data)) {
    _pendingStart = false;
    _start = false;
}

Json::Value PlayRoom::getInfo(const string &key) const {
    misc::logger(typeid(*this).name(), "Try getting info");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return key.empty() ? _info : _info[key];
}

Json::Value PlayRoom::getData(const string &key) const {
    misc::logger(typeid(*this).name(), "Try getting data");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return key.empty() ? _data : _data[key];
}

void PlayRoom::setInfo(const string &key, Json::Value value) {
    misc::logger(typeid(*this).name(), "Try setting info");
    unique_lock<shared_mutex> lock(_sharedMutex);
    _info[key] = move(value);
}

void PlayRoom::setData(const string &key, Json::Value value) {
    misc::logger(typeid(*this).name(), "Try setting data");
    unique_lock<shared_mutex> lock(_sharedMutex);
    _data[key] = move(value);
}

bool PlayRoom::getPendingStart() const {
    misc::logger(typeid(*this).name(), "Try getting pending start");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _pendingStart;
}

void PlayRoom::setPendingStart(const bool &pendingStart) {
    misc::logger(typeid(*this).name(), "Try setting pending start" + to_string(pendingStart));
    unique_lock<shared_mutex> lock(_sharedMutex);
    _pendingStart = pendingStart;
}

bool PlayRoom::getStart() const {
    misc::logger(typeid(*this).name(), "Try getting start");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _start;
}

void PlayRoom::setStart(const bool &start) {
    misc::logger(typeid(*this).name(), "Try setting start" + to_string(start));
    unique_lock<shared_mutex> lock(_sharedMutex);
    _start = start;
}

bool PlayRoom::checkReady() const {
    misc::logger(typeid(*this).name(), "Try checking ready");
    bool ready = true;
    shared_lock<shared_mutex> lock(_sharedMutex);
    if (_connectionsMap.empty()) {
        return false;
    }
    for (auto &pair : _connectionsMap) {
        if (!pair.second->getContext<Play>()->getReady()) {
            ready = false;
            break;
        }
    }
    misc::logger(typeid(*this).name(), "Check ready: " + to_string(ready));
    return ready;
}

void PlayRoom::resetReady() {
    misc::logger(typeid(*this).name(), "Try resetting ready");
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        pair.second->getContext<Play>()->setReady(false);
    }
}

bool PlayRoom::checkPassword(const std::string &password) const {
    misc::logger(typeid(*this).name(), "Try checking password" + password);
    shared_lock<shared_mutex> lock(_sharedMutex);
    return crypto::blake2b(password, 1) == _encryptedPassword;
}

void PlayRoom::publish(const uint64_t &action, Json::Value &&message, const uint64_t &excluded) {
    misc::logger(typeid(*this).name(), "Try exclude broadcasting" + websocket::fromJson(message));
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            if (!excluded || excluded != pair.second->getContext<Play>()->getSid()) {
                pair.second->send(websocket::fromJson(message));
            }
        }
    }
    if (action == 4) {
        _setHistory(move(message["data"]));
    }
}

Json::Value PlayRoom::getHistory(const unsigned int &begin, const unsigned int &count) const {
    misc::logger(typeid(*this).name(), "Try getting histories" + to_string(count));
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    if (begin < _history.size()) {
        auto end = begin + count > _history.size() ? _history.size() : begin + count;
        for (unsigned int iter = begin; iter < end; ++iter) {
            result.append(_history[iter]);
        }
    }
    misc::logger(typeid(*this).name(), "Histories: " + websocket::fromJson(result));
    return result;
}

void PlayRoom::_setHistory(Json::Value &&data) {
    misc::logger(typeid(*this).name(), "Try setting histories: " + websocket::fromJson(data));
    unique_lock<shared_mutex> lock(_sharedMutex);
    _history.push_back(move(data));
    if (_history.size() > _maxHistoryCount) {
        _history.pop_front();
    }
}

Json::Value PlayRoom::parseInfo() const {
    misc::logger(typeid(*this).name(), "Try parsing info");
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value info;
    info["rid"] = _rid;
    info["roomInfo"] = _info;
    info["private"] = crypto::blake2b("", 1) != _encryptedPassword;
    info["start"] = _start;
    info["count"] = _connectionsMap.size();
    info["capacity"] = _capacity;
    misc::logger(typeid(*this).name(), "Info: " + websocket::fromJson(info));
    return info;
}

Json::Value PlayRoom::getPlayers() const {
    misc::logger(typeid(*this).name(), "Try getting players");
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        Json::Value tempInfo;
        auto play = pair.second->getContext<Play>();
        auto info = play->getInfo();

        tempInfo["sid"] = play->getSid();
        tempInfo["uid"] = info.getValueOfId();
        tempInfo["username"] = info.getValueOfUsername();
        tempInfo["config"] = play->getConfig();
        tempInfo["ready"] = play->getReady();
        result.append(tempInfo);
    }
    misc::logger(typeid(*this).name(), "Players: " + websocket::fromJson(result));
    return result;
}
