//
// Created by Parti on 2021/02/27.
//

#include <structures/BaseRoom.h>
#include <structures/BasePlayer.h>

using namespace tech::structures;
using namespace tech::utils;
using namespace drogon;
using namespace std;

BaseRoom::BaseRoom(BaseRoom &&room) noexcept {
    _id = room._id;
    _capacity = room._capacity;
    _count = room._count;
    _cycleID = room._cycleID;
}

BaseRoom::BaseRoom(
        string &&id,
        const uint64_t &capacity
) : _id(id), _capacity(capacity) {
    _count = 0;
    _cycleID = 0;
}

void BaseRoom::subscribe(WebSocketConnectionPtr connection) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    auto sidsMap = connection->getContext<BasePlayer>()->getSidsMap();
    if (sidsMap->count(_id)) {
        throw range_error("Room already subscribed");
    }
    if (_count == _capacity) {
        throw range_error("Room is isFull");
    }
    ++_count;
    auto sid = _loopCycleID();
    sidsMap->insert(make_pair(_id, sid));
    _connectionsMap[sid] = move(connection);
}

void BaseRoom::unsubscribe(const WebSocketConnectionPtr &connection) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    auto sidsMap = connection->getContext<BasePlayer>()->getSidsMap();
    if (!sidsMap->count(_id)) {
        throw out_of_range("Room not subscribed");
    }
    _connectionsMap.erase(sidsMap->at(_id));
    sidsMap->erase(_id);
    --_count;
}

bool BaseRoom::isEmpty() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.empty();
}

bool BaseRoom::isFull() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _count == _capacity;
}

uint64_t BaseRoom::getCount() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _count;
}

uint64_t BaseRoom::getCapacity() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _capacity;
}

bool BaseRoom::operator==(const BaseRoom &room) const {
    return _id == room._id;
}

uint64_t BaseRoom::_loopCycleID() {
    while (true) {
        if (!_connectionsMap.count(_cycleID)) {
            return _cycleID;
        }
        _cycleID = _cycleID + 1 == _capacity ? 0 : _cycleID + 1;
    }
}