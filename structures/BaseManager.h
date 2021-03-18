//
// Created by Parti on 2020/12/7.
//

#pragma once

#include <shared_mutex>
#include <utils/websocket.h>

namespace tech::structures {
    template<class RoomType>
    class BaseManager {
    public:
        typedef struct __RoomWithMutex {
            __RoomWithMutex(RoomType &&room) :
                    room(std::move(room)) {}

            RoomType room;
            mutable std::shared_mutex sharedMutex;
        } RoomWithMutex;
        typedef struct __SharedRoom {
            __SharedRoom(const RoomType &room, std::shared_lock<std::shared_mutex> &&lock) :
                    room(room), lock(std::move(lock)) {}

            RoomType room;
            std::shared_lock<std::shared_mutex> lock;
        } SharedRoom;
        typedef struct __UniqueRoom {
            __UniqueRoom(const RoomType &room, std::unique_lock<std::shared_mutex> &&lock) :
                    room(room), lock(std::move(lock)) {}

            RoomType room;
            std::unique_lock<std::shared_mutex> lock;
        } UniqueRoom;

        virtual void subscribe(const std::string &rid, drogon::WebSocketConnectionPtr connection) = 0;

        virtual void unsubscribe(const std::string &rid, const drogon::WebSocketConnectionPtr &connection) = 0;

        [[maybe_unused]] size_t getSize() const {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            return _idsMap.size();
        }

        SharedRoom &getSharedRoom(const std::string &rid) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(rid);
            if (iter != _idsMap.end()) {
                return SharedRoom(
                        iter->second.room,
                        std::move(std::shared_lock<std::shared_mutex>(iter->second.sharedMutex))
                );
            }
            throw std::out_of_range("Room not found");
        }

        UniqueRoom &getUniqueRoom(const std::string &rid) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(rid);
            if (iter != _idsMap.end()) {
                return {
                        iter->second.room,
                        std::move(std::unique_lock<std::shared_mutex>(iter->second.sharedMutex))
                };
            }
            throw std::out_of_range("Room not found");
        }

        void createRoom(RoomType &&room) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            const std::string &id = room.getID();
            auto[itr, inserted] = _idsMap.try_emplace(
                    std::move(id),
                    RoomWithMutex(std::move(room))
            );
            if (!inserted) {
                throw std::overflow_error("Room already subscribed");
            }
        }

        void removeRoom(const std::string &rid) {
            typename decltype(_idsMap)::node_type node;
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(rid);
            if (iter != _idsMap.end()) {
                std::unique_lock<std::shared_mutex>(iter->second.sharedMutex);
                node = _idsMap.extract(rid);
                if (node.empty()) {
                    LOG_INFO << "Room " << rid << " already removed";
                }
            } else {
                LOG_INFO << "Room " << rid << " does not exist";
            }
        }

        virtual ~BaseManager() noexcept {};

    protected:
        std::unordered_map<std::string, RoomWithMutex> _idsMap;
        mutable std::shared_mutex _sharedMutex;
    };
}

