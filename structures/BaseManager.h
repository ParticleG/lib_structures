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
        class RoomWithLock {
        public:
            RoomWithLock(RoomType &room_, std::shared_lock<std::shared_mutex> &&lock_):
                room(room_), lock(std::move(lock_)) {}
            RoomType *operator->() const {
                return &room;
            }
        private:
            RoomType &room;
            std::shared_lock<std::shared_mutex> lock;
        };
        virtual void subscribe(const std::string &rid, drogon::WebSocketConnectionPtr connection) = 0;

        virtual void unsubscribe(const std::string &rid, const drogon::WebSocketConnectionPtr &connection) = 0;

        [[maybe_unused]] size_t getSize() const {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            return _idsMap.size();
        }

        RoomWithLock getRoom(const std::string &rid) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(rid);
            if (iter != _idsMap.end()) {
                return {iter->second, std::move(lock)};
            }
            throw std::out_of_range("Room not found");
        }

        void createRoom(RoomType &&room) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            const std::string &id = room.getID();
            auto [itr, inserted] = _idsMap.try_emplace(std::move(id), std::move(room));
            if (!inserted) {
                throw std::overflow_error("Room already subscribed");
            }
        }

        void removeRoom(const std::string &rid) {
            typename decltype(_idsMap)::node_type node;
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            node = _idsMap.extract(rid);
            if (node.empty()) {
                LOG_INFO << "Room " << rid << " already removed";
            }
        }

        virtual ~BaseManager() noexcept {};

    protected:
        std::unordered_map<std::string, RoomType> _idsMap;
        mutable std::shared_mutex _sharedMutex;
    };
}

