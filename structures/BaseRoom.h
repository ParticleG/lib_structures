//
// Created by Parti on 2021/02/27.
//

#pragma once

#include <shared_mutex>
#include <utils/websocket.h>

namespace tech::structures {
    class BaseRoom : public trantor::NonCopyable {
    public:
        explicit BaseRoom(const BaseRoom &room);

        explicit BaseRoom(BaseRoom &&room) noexcept;

        explicit BaseRoom(
                std::string &&id,
                const uint64_t &capacity
        );

        void subscribe(drogon::WebSocketConnectionPtr connection);

        void unsubscribe(const drogon::WebSocketConnectionPtr &connection);

        bool isEmpty() const;

        bool isFull() const;

        std::string getID() const;

        uint64_t getCount() const;

        uint64_t getCapacity() const;

        bool operator==(const BaseRoom &room) const;

        virtual ~BaseRoom() noexcept {};

    protected:
        const std::string _id;
        uint64_t _count, _capacity, _cycleID;
        std::unordered_map<uint64_t, drogon::WebSocketConnectionPtr> _connectionsMap;
        mutable std::shared_mutex _sharedMutex;

        uint64_t _loopCycleID();
    };
}

