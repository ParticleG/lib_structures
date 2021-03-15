//
// Created by Parti on 2020/12/7.
//

#pragma once

#include <parallel_hashmap/parallel_hashmap/phmap.h>
#include <shared_mutex>
#include <utils/websocket.h>

namespace tech::structures {
    template<class RoomType>
    class BaseManager {
    public:
        using RoomMapWithMutex =
        phmap::parallel_flat_hash_map<std::string, RoomType,
                phmap::priv::hash_default_hash<std::string>,
                phmap::priv::hash_default_eq<std::string>,
                std::allocator<std::pair<const std::string, RoomType>>,
                4,
                std::mutex>;

        virtual void subscribe(const std::string &rid, drogon::WebSocketConnectionPtr connection) = 0;

        virtual void unsubscribe(const std::string &rid, const drogon::WebSocketConnectionPtr &connection) = 0;

        [[maybe_unused]] size_t getSize() const {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            return _idsMap.size();
        }

        RoomType &getRoom(const std::string &rid) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            RoomType room;
            if (_idsMap.if_contains(rid, [&](const RoomType &val) {
                room = val;
            })) {
                return room;
            }
            throw std::out_of_range("Room not found");
        }

        void createRoom(RoomType &&room) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            const std::string &id = room.getID();
            auto[itr, inserted] = _idsMap.try_emplace(std::move(id), std::move(room));
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
        RoomMapWithMutex _idsMap;
        mutable std::shared_mutex _sharedMutex;
    };
}

