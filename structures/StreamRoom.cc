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
        unordered_map<uint64_t, bool> players,
        const uint64_t &capacity
) : BaseRoom(move(srid), capacity),
    _playRid(std::move(playRid)),
    _innerPlace(players.size()),
    _players(move(players)),
    _seed(misc::uniform_random()) {
    _start = false;
    _finish = false;
}

void StreamRoom::subscribe(WebSocketConnectionPtr connection) {
    misc::logger(typeid(*this).name(), "Try subscribing connection");
    auto player = connection->getContext<Stream>();
    if (player->isSingleSid() && !get<string>(player->getRid()).empty()) {
        throw length_error("Can only subscribe one room");
    }
    if (isFull()) {
        throw range_error("Room is full");
    }

    unique_lock<shared_mutex> lock(_sharedMutex);
    if (get<string>(player->getRid()) == _rid) {
        throw overflow_error("Room already subscribed");
    }
    auto sid = _loopCycleID();
    player->setSid(_rid, sid);
    _connectionsMap[sid] = move(connection);
    try {
        if (!_players.at(player->getUid())) {
            _players[player->getUid()] = true;
        }
    } catch (...) {
        misc::logger(typeid(*this).name(), "Spectator: (" + to_string(player->getUid()) + ") " + to_string(sid));
    }
    misc::logger(typeid(*this).name(), "Subscribe: (" + _rid + ") " + to_string(sid));
}

void StreamRoom::publish(Json::Value &&message, const uint64_t &excluded) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        if (!excluded || excluded != pair.second->getContext<Stream>()->getSid()) {
            pair.second->send(websocket::fromJson(message));
        }
    }
}

Json::Value StreamRoom::parseInfo() const {
    Json::Value info;
    shared_lock<shared_mutex> lock(_sharedMutex);
    info["rid"] = _rid;
    info["count"] = _connectionsMap.size();
    info["capacity"] = _capacity;
    return info;
}

Json::Value StreamRoom::parseHistories() const {
    Json::Value result;
    result["history"] = Json::arrayValue;
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        result["history"].append(pair.second->getContext<Stream>()->parseHistory());
    }
    return result;
}

Json::Value StreamRoom::getPlayers() const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        result.append(pair.second->getContext<Stream>()->parsePlayerInfo(Json::objectValue));
    }
    return result;
}

std::string StreamRoom::getPlayRid() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _playRid;
}

uint64_t StreamRoom::getSeed() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _seed;
}

bool StreamRoom::checkIfPlaying(const int64_t &uid) const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _players.count(uid);
}

bool StreamRoom::hasConnection() const {
    bool result = false;
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _players) {
        if (pair.second) {
            result = true;
            break;
        }
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

bool StreamRoom::checkReady() const {
    bool ready = true;
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _players) {
        if (!pair.second) {
            ready = false;
            break;
        }
    }
    return ready;
}

bool StreamRoom::checkFinished() const {
    uint64_t finished{}, playerCount{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        auto stream = pair.second->getContext<Stream>();
        if (stream->getDead()) {
            finished++;
        }
        if (!stream->getSpectate()) {
            playerCount++;
        }
    }
    return finished + 1 >= playerCount;
}

Json::Value StreamRoom::getDeaths() const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
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
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _finish;
}

void StreamRoom::setFinish(const bool &finish) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _finish = finish;
}
