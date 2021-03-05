//
// Created by Parti on 2021/2/27.
//

#include <structures/PlayRoom.h>
#include <utils/Crypto.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

PlayRoom::PlayRoom(
        string id,
        string type,
        string name,
        const string &password,
        const uint64_t &capacity
) : BaseRoom(move(id), capacity),
    _type(move(type)),
    _name(move(name)),
    _encryptedPassword(Crypto::blake2b(password, 1)) {
    _pendingStart = false;
    _start = false;
}

string PlayRoom::getType() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _type;
}

bool PlayRoom::getPendingStart() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _pendingStart;
}

void PlayRoom::setPendingStart(const bool &pendingStart) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _pendingStart = pendingStart;
}

bool PlayRoom::getStart() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _start;
}

void PlayRoom::setStart(const bool &start) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _start = start;
}

bool PlayRoom::checkPassword(const std::string &password) const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return Crypto::blake2b(password, 1) == _encryptedPassword;
}

void PlayRoom::publish(const uint64_t &action, Json::Value &&message) {
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            pair.second->send(WebSocket::fromJson(message));
        }
    }
    if (action == 4) {
        _setHistory(move(message["data"]));
    }
}

void PlayRoom::publish(const uint64_t &action, Json::Value &&message, const uint64_t &excluded) {
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            if (excluded != pair.second->getContext<Play>()->getSidsMap()->at(_id)) {
                pair.second->send(WebSocket::fromJson(message));
            }
        }
    }
    if (action == 4) {
        _setHistory(move(message["data"]));
    }
}

Json::Value PlayRoom::getHistory(const unsigned int &begin, const unsigned int &count) const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    if (begin < _history.size()) {
        auto end = begin + count > _history.size() ? _history.size() : begin + count;
        for (unsigned int iter = begin; iter < end; ++iter) {
            result.append(_history[iter]);
        }
    }
    return result;
}

void PlayRoom::_setHistory(Json::Value &&data) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _history.push_back(move(data));
    if (_history.size() > _maxHistoryCount) {
        _history.pop_front();
    }
}

Json::Value PlayRoom::parseInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value info;
    info["rid"] = _id;
    info["name"] = _name;
    info["type"] = _type;
    info["private"] = !checkPassword("");
    info["start"] = getStart();
    info["count"] = _count;
    info["capacity"] = _capacity;
    return info;
}

Json::Value PlayRoom::getPlayers() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        Json::Value tempInfo;
        auto play = pair.second->getContext<Play>();
        auto info = play->getInfo();

        tempInfo["sid"] = play->getSidsMap()->begin()->second;
        tempInfo["uid"] = info->getValueOfId();
        tempInfo["username"] = info->getValueOfUsername();
        tempInfo["config"] = play->getConfig();
        tempInfo["ready"] = play->getReady();
        result.append(tempInfo);
    }
    return result;
}

