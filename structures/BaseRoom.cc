//
// Created by Parti on 2021/02/27.
//

#include <structures/BaseRoom.h>
#include <structures/BasePlayer.h>
#include <utils/misc.h>

using namespace tech::structures;
using namespace tech::utils;
using namespace drogon;
using namespace std;

BaseRoom::BaseRoom(const BaseRoom &room) :
        _rid(room._rid) {
    _capacity = room._capacity;
    _cycleID = room._cycleID;
}

BaseRoom::BaseRoom(BaseRoom &&room) noexcept:
        _rid(room._rid) {
    _capacity = room._capacity;
    _cycleID = room._cycleID;
}

BaseRoom::BaseRoom(
        string &&id,
        const uint64_t &capacity
) : _rid(id), _capacity(capacity) {
    _cycleID = 1;
}

void BaseRoom::subscribe(WebSocketConnectionPtr connection) {
    misc::logger(typeid(*this).name(), "Try subscribing connection");
    auto player = connection->getContext<BasePlayer>();
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
    misc::logger(typeid(*this).name(), "Subscribe: (" + _rid + ") " + to_string(sid));
}

void BaseRoom::unsubscribe(const WebSocketConnectionPtr &connection) {
    misc::logger(typeid(*this).name(), "Try unsubscribing connection");
    auto player = connection->getContext<BasePlayer>();
    unique_lock<shared_mutex> lock(_sharedMutex);
    if (get<string>(player->getRid()) != _rid) {
        throw underflow_error("Room not subscribed");
    }
    misc::logger(typeid(*this).name(), "Unsubscribe: (" + _rid + ") " + to_string(player->getSid(_rid)));
    _connectionsMap.erase(player->getSid(_rid));
    player->setSid(_rid);
}

bool BaseRoom::isEmpty() const {
    misc::logger(typeid(*this).name(), "Try checking empty");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.empty();
}

bool BaseRoom::isFull() const {
    misc::logger(typeid(*this).name(), "Try checking full");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.size() == _capacity;
}

std::string BaseRoom::getRID() const {
    misc::logger(typeid(*this).name(), "Try getting rid");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _rid;
}

uint64_t BaseRoom::getCount() const {
    misc::logger(typeid(*this).name(), "Try getting count");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.size();
}

uint64_t BaseRoom::getCapacity() const {
    misc::logger(typeid(*this).name(), "Try getting capacity");
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _capacity;
}

bool BaseRoom::operator==(const BaseRoom &room) const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _rid == room._rid;
}

uint64_t BaseRoom::_loopCycleID() {
    misc::logger(typeid(*this).name(), "Try generate sid");
    while (true) {
        if (!_connectionsMap.count(_cycleID)) {
            return _cycleID;
        }
        _cycleID = _cycleID + 1 > _capacity ? 1 : _cycleID + 1;
    }
}
