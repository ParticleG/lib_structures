//
// Created by Parti on 2021/2/27.
//

#include <structures/StreamRoom.h>
#include <utils/misc.h>

#include <utility>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

StreamRoom::StreamRoom(
        string playRid,
        string srid,
        const uint64_t &initCount,
        const uint64_t &capacity
) : BaseRoom(move(srid), capacity), _playRid(std::move(playRid)), _initCount(initCount), _innerPlace(initCount) {
    _start = false;
    _finish = false;
}

void StreamRoom::publish(Json::Value &&message) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        pair.second->send(websocket::fromJson(message));
    }
}

void StreamRoom::publish(Json::Value &&message, const uint64_t &excluded) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        if (excluded != pair.second->getContext<Stream>()->getSid()) {
            pair.second->send(websocket::fromJson(message));
        }
    }
}

Json::Value StreamRoom::parseInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value info;
    info["rid"] = _rid;
    info["count"] = _count;
    info["capacity"] = _capacity;
    return info;
}

Json::Value StreamRoom::getPlayers() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        result.append(pair.second->getContext<Stream>()->parsePlayerInfo(Json::objectValue));
    }
    return result;
}

std::string StreamRoom::getPlayRid() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _playRid;
}

bool StreamRoom::getStart() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _start;
}

void StreamRoom::setStart(const bool &start) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _start = start;
}

bool StreamRoom::checkReady() const {
    int ready = 0;
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        if (!pair.second->getContext<Stream>()->getWatch()) {
            ready++;
        }
    }
    return ready == _initCount;
}

bool StreamRoom::checkFinished() const {
    int finished = 0;
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        if (pair.second->getContext<Stream>()->getDead()) {
            finished++;
        }
    }
    return (finished >= (_count - 1 < 0 ? 0 : _count - 1));
}

Json::Value StreamRoom::getDeaths() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result(Json::arrayValue);
    for (auto &pair : _connectionsMap) {
        auto stream = pair.second->getContext<Stream>();
        Json::Value tempInfo;
        tempInfo["uid"] = stream->getUid();
        tempInfo["place"] = stream->getPlace() ? stream->getPlace() : 1;
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

bool StreamRoom::getFinish() const {
    return _finish;
}

void StreamRoom::setFinish(const bool &finish) {
    _finish = finish;
}
