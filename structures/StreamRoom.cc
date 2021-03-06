//
// Created by Parti on 2021/2/27.
//

#include <structures/StreamRoom.h>
#include <utils/Utils.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

StreamRoom::StreamRoom(
        string rid,
        const uint64_t &capacity
) : BaseRoom(move(rid), capacity), _innerPlace(capacity) {
    _start = false;
}

void StreamRoom::publish(Json::Value &&message) {
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            pair.second->send(WebSocket::fromJson(message));
        }
    }
}

void StreamRoom::publish(Json::Value &&message, const uint64_t &excluded) {
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            if (excluded != pair.second->getContext<Stream>()->getSidsMap()->at(_id)) {
                pair.second->send(WebSocket::fromJson(message));
            }
        }
    }
}

Json::Value StreamRoom::parseInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value info;
    info["rid"] = _id;
    info["count"] = _count;
    info["capacity"] = _capacity;
    return info;
}

Json::Value StreamRoom::getPlayers() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        Json::Value tempInfo;
        tempInfo["uid"] = pair.second->getContext<Stream>()->getUid();
        result.append(tempInfo);
    }
    return result;
}

bool StreamRoom::getStart() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _start;
}

void StreamRoom::setStart(const bool &start) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _start = start;
}

bool StreamRoom::checkFinished() const {
    bool finished = true;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &pair : _connectionsMap) {
            if (!pair.second->getContext<Stream>()->getDead()) {
                finished = false;
                break;
            }
        }
    }
    return finished;
}

Json::Value StreamRoom::getDeaths() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        auto stream = pair.second->getContext<Stream>();
        Json::Value tempInfo;
        tempInfo["uid"] = stream->getUid();
        tempInfo["place"] = stream->getPlace();
        tempInfo["score"] = stream->getScore();
        tempInfo["survivalTime"] = stream->getSurvivalTime();
        result.append(tempInfo);
    }
    return result;
}

uint64_t StreamRoom::generatePlace() {
    unique_lock<shared_mutex> lock(_sharedMutex);
    return _innerPlace--;
}
