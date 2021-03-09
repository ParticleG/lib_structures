//
// Created by Parti on 2021/2/27.
//

#include "structures/BaseRoom.h"
#include "structures/Stream.h"

namespace tech::structures {
    class StreamRoom : public BaseRoom {
    public:
        explicit StreamRoom(
                std::string rid,
                const uint64_t &capacity
        );

        void publish(Json::Value &&message);

        void publish(Json::Value &&message, const uint64_t &excluded);

        Json::Value parseInfo() const;

        Json::Value getPlayers() const;

        bool getStart() const;

        void setStart(const bool &start);

        bool checkFinished() const;

        Json::Value getDeaths() const;

        uint64_t generatePlace();

    private:
        uint64_t _innerPlace;
        bool _start;
    };
}