//
// Created by Parti on 2021/2/27.
//

#include <structures/ChatRoom.h>
#include <utils/Utils.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

ChatRoom::ChatRoom(
        string id,
        const uint64_t &capacity,
        const unsigned int &maxHistoryCount
) : BaseRoom(move(id), capacity), _maxHistoryCount(maxHistoryCount) {}

void ChatRoom::publish(Json::Value &&message) {
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            pair.second->send(WebSocket::fromJson(message));
        }
    }
    _setHistory(move(message["data"]));
}

Json::Value ChatRoom::getHistory(const unsigned int &begin, const unsigned int &count) const {
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

void ChatRoom::_setHistory(Json::Value &&data) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _history.push_back(move(data));
    if (_history.size() > _maxHistoryCount) {
        _history.pop_front();
    }
}

Json::Value ChatRoom::parseInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value info;
    info["rid"] = _id;
    info["count"] = _count;
    info["capacity"] = _capacity;
    return Json::Value();
}


