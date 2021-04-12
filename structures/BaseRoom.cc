//
// Created by Parti on 2021/02/27.
//

#include <structures/BaseRoom.h>
#include <structures/BasePlayer.h>

using namespace tech::structures;
using namespace tech::utils;
using namespace drogon;
using namespace std;

BaseRoom::BaseRoom(const BaseRoom &room) :
        _rid(room._rid) {
    _capacity = room._capacity;
    _count = room._count;
    _cycleID = room._cycleID;
}

BaseRoom::BaseRoom(BaseRoom &&room) noexcept:
        _rid(room._rid) {
    _capacity = room._capacity;
    _count = room._count;
    _cycleID = room._cycleID;
}

BaseRoom::BaseRoom(
        string &&id,
        const uint64_t &capacity
) : _rid(id), _capacity(capacity) {
    _count = 0;
    _cycleID = 0;
}

void BaseRoom::subscribe(WebSocketConnectionPtr connection) {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try subscribing connection";
    unique_lock <shared_mutex> lock(_sharedMutex);
    auto player = connection->getContext<BasePlayer>();
    if (player->isSingleSid() && !get<string>(player->getRid()).empty()) {
        throw length_error("Can only subscribe one room");
    }
    if (get<string>(player->getRid()) == _rid) {
        throw overflow_error("Room already subscribed");
    }
    if (_count == _capacity) {
        throw range_error("Room is full");
    }
    ++_count;
    auto sid = _loopCycleID();
    player->setSid(_rid, sid);
    _connectionsMap[sid] = move(connection);
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Subscribe: (" << _rid << ") " << sid;
}

void BaseRoom::unsubscribe(const WebSocketConnectionPtr &connection) {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try unsubscribing connection";
    unique_lock <shared_mutex> lock(_sharedMutex);
    auto player = connection->getContext<BasePlayer>();
    if (get<string>(player->getRid()) != _rid) {
        throw underflow_error("Room not subscribed");
    }
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Unsubscribe: (" << _rid << ") " << player->getSid(_rid);
    _connectionsMap.erase(player->getSid(_rid));
    player->setSid(_rid);
    --_count;

}

bool BaseRoom::isEmpty() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try checking empty";
    shared_lock <shared_mutex> lock(_sharedMutex);
    return _connectionsMap.empty();
}

bool BaseRoom::isFull() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try checking full";
    shared_lock <shared_mutex> lock(_sharedMutex);
    return _count == _capacity;
}

std::string BaseRoom::getRID() const {
    LOG_DEBUG << "(" << GetCurrentThreadId() << ")[" << typeid(*this).name() << "] Try getting rid";
    shared_lock <shared_mutex> lock(_sharedMutex);
    return _rid;
}

uint64_t BaseRoom::getCount() const {
    shared_lock <shared_mutex> lock(_sharedMutex);
    return _count;
}

uint64_t BaseRoom::getCapacity() const {
    shared_lock <shared_mutex> lock(_sharedMutex);
    return _capacity;
}

bool BaseRoom::operator==(const BaseRoom &room) const {
    shared_lock <shared_mutex> lock(_sharedMutex);
    return _rid == room._rid;
}

uint64_t BaseRoom::_loopCycleID() {
    while (true) {
        if (!_connectionsMap.count(_cycleID)) {
            return _cycleID;
        }
        _cycleID = _cycleID + 1 == _capacity ? 0 : _cycleID + 1;
    }
}
