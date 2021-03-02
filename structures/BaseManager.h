//
// Created by Parti on 2020/12/7.
//

#pragma once

#include <shared_mutex>
#include <utils/WebSocket.h>

namespace tech::structures {
    template<class RoomType>
    class BaseManager : public trantor::NonCopyable {
    public:
        uint64_t subscribe(const std::string &id, drogon::WebSocketConnectionPtr connection) {
            try {
                return _subscribeToRoom(id, move(connection));
            } catch (std::range_error &error) {
                LOG_WARN << error.what();
                throw error;
            }
        }

        void unsubscribe(const std::string &id, const uint64_t &sid) {
            {
                std::shared_lock<std::shared_mutex> lock(_sharedMutex);
                auto iter = _idsMap.find(id);
                if (iter == _idsMap.end()) {
                    throw std::out_of_range("Room not found");
                }
                iter->second->unsubscribe(sid);
                if (!iter->second->isEmpty())
                    return;
            }
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(id);
            if (iter == _idsMap.end()) {
                throw std::out_of_range("Room not found");
            }
            if (iter->second->isEmpty())
                _idsMap.erase(iter);
        }

        size_t getSize() const {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            return _idsMap.size();
        }

        std::shared_ptr<RoomType> getRoom(const std::string &id) const {
            std::shared_lock <std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(id);
            if (iter != _idsMap.end()) {
                return iter->second;
            }
            throw std::out_of_range("Room not found");
        }

        uint64_t getCapacity(const std::string &id) const {
            std::shared_lock <std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(id);
            if (iter != _idsMap.end()) {
                return iter->second->getCapacity();
            }
            throw std::out_of_range("Room not found");
        }

        void createRoom(std::string &&id, RoomType &&room) {
            std::unique_lock <std::shared_mutex> lock(_sharedMutex);
            _idsMap[std::move(id)] = std::move(std::make_shared<RoomType>(std::move(room)));
        }

        void removeRoom(const std::string &id) {
            std::unique_lock <std::shared_mutex> lock(_sharedMutex);
            _idsMap.erase(id);
        }

    protected:
        std::unordered_map<std::string, std::shared_ptr<RoomType>> _idsMap;
        mutable std::shared_mutex _sharedMutex;

        uint64_t _subscribeToRoom(const std::string &id, drogon::WebSocketConnectionPtr &&connection) {
            {
                std::shared_lock <std::shared_mutex> lock(_sharedMutex);
                auto iter = _idsMap.find(id);
                if (iter != _idsMap.end()) {
                    try {
                        return iter->second->subscribe(move(connection));
                    } catch (std::range_error &error) {
                        LOG_WARN << error.what();
                        throw error;
                    }
                }
            }
            std::unique_lock <std::shared_mutex> lock(_sharedMutex);
            auto iter = _idsMap.find(id);
            if (iter != _idsMap.end()) {
                return iter->second->subscribe(move(connection));
            }
            throw std::out_of_range("Room not found");
        }
    };
}

