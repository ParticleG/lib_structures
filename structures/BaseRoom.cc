//
// Created by Parti on 2021/02/27.
//

#include <structures/BaseRoom.h>

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

uint64_t BaseRoom::subscribe(WebSocketConnectionPtr handler) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    if (_count == _capacity) {
        throw range_error("Room is isFull");
    }
    ++_count;
    auto tempID = _loopCycleID();
    _connectionsMap[tempID] = move(handler);
    return tempID;
}

void BaseRoom::unsubscribe(const uint64_t &id) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _connectionsMap.erase(id);
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