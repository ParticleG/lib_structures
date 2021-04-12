//
// Created by Parti on 2021/2/27.
//

#include <structures/PlayRoom.h>
#include <utils/crypto.h>

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
    _encryptedPassword(crypto::blake2b(password, 1)) {
    _pendingStart = false;
    _start = false;
}

string PlayRoom::getType() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try getting type";
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _type;
}

bool PlayRoom::getPendingStart() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try getting pending start";
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _pendingStart;
}

void PlayRoom::setPendingStart(const bool &pendingStart) {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try setting pending start: " << pendingStart;
    unique_lock<shared_mutex> lock(_sharedMutex);
    _pendingStart = pendingStart;
}

bool PlayRoom::getStart() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try getting start";
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _start;
}

void PlayRoom::setStart(const bool &start) {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try setting start" << start;
    unique_lock<shared_mutex> lock(_sharedMutex);
    _start = start;
}

bool PlayRoom::checkReady() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try checking ready";
    bool ready = true;
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        if (!pair.second->getContext<Play>()->getReady()) {
            ready = false;
            break;
        }
    }
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Check ready: " << ready;
    return ready;
}

void PlayRoom::resetReady() {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try resetting ready";
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        pair.second->getContext<Play>()->setReady(false);
    }
}

bool PlayRoom::checkPassword(const std::string &password) const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try checking password" << password;
    shared_lock<shared_mutex> lock(_sharedMutex);
    return crypto::blake2b(password, 1) == _encryptedPassword;
}

void PlayRoom::publish(const uint64_t &action, Json::Value &&message) {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try broadcasting" << websocket::fromJson(message);
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            pair.second->send(websocket::fromJson(message));
        }
    }
    if (action == 4) {
        _setHistory(move(message["data"]));
    }
}

void PlayRoom::publish(const uint64_t &action, Json::Value &&message, const uint64_t &excluded) {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try exclude broadcasting" << websocket::fromJson(message);
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            if (excluded != pair.second->getContext<Play>()->getSid()) {
                pair.second->send(websocket::fromJson(message));
            }
        }
    }
    if (action == 4) {
        _setHistory(move(message["data"]));
    }
}

Json::Value PlayRoom::getHistory(const unsigned int &begin, const unsigned int &count) const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try getting histories" << count;
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
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try setting histories" << websocket::fromJson(data);
    unique_lock<shared_mutex> lock(_sharedMutex);
    _history.push_back(move(data));
    if (_history.size() > _maxHistoryCount) {
        _history.pop_front();
    }
}

Json::Value PlayRoom::parseInfo() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try parsing info";
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value info;
    info["rid"] = _rid;
    info["name"] = _name;
    info["type"] = _type;
    info["private"] = !checkPassword("");
    info["start"] = getStart();
    info["count"] = _count;
    info["capacity"] = _capacity;
    return info;
}

Json::Value PlayRoom::getPlayers() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try getting players";
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        Json::Value tempInfo;
        auto play = pair.second->getContext<Play>();
        auto info = play->getInfo();

        tempInfo["sid"] = play->getSid();
        tempInfo["uid"] = info->getValueOfId();
        tempInfo["username"] = info->getValueOfUsername();
        tempInfo["config"] = play->getConfig();
        tempInfo["ready"] = play->getReady();
        result.append(tempInfo);
    }
    return result;
}
