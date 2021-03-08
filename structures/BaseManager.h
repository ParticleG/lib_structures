//
// Created by Parti on 2020/12/7.
//

#pragma once

#include <shared_mutex>
#include <utils/WebSocket.h>

namespace tech::structures {
    template<class RoomType>
    class BaseManager {
    public:
        virtual void subscribe(const std::string &rid, drogon::WebSocketConnectionPtr connection) = 0;

        virtual void unsubscribe(const std::string &rid, const drogon::WebSocketConnectionPtr &connection) = 0;

        [[maybe_unused]] size_t getSize() const {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            return _idsMap.size();
        }

        std::shared_ptr<RoomType> getRoom(const std::string &rid) const {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(rid);
            if (iter != _idsMap.end()) {
                return iter->second;
            }
            throw std::out_of_range("Room not found");
        }

        void createRoom(RoomType &&room) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            if (_idsMap.find(room.getID()) != _idsMap.end()) {
                throw std::overflow_error("Room already subscribed");
            }
            _idsMap[room.getID()] = std::move(std::make_shared<RoomType>(std::move(room)));
        }

        void removeRoom(const std::string &rid) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            if (!_idsMap.erase(rid)) {
                LOG_INFO << "Room " << rid << " already removed";
            }
        }

    protected:
        std::unordered_map<std::string, std::shared_ptr<RoomType>> _idsMap;
        mutable std::shared_mutex _sharedMutex;
    };
}

