//
// Created by Parti on 2021/2/27.
//

#include <structures/BaseRoom.h>
#include <structures/Stream.h>

namespace tech::structures {
    class StreamRoom : public BaseRoom {
    public:
        explicit StreamRoom(
                std::string playRid,
                std::string srid,
                std::unordered_map<uint64_t, bool> players,
                const uint64_t &capacity
        );

        void subscribe(drogon::WebSocketConnectionPtr connection) override;

        void publish(Json::Value &&message, const uint64_t &excluded = 0);

        Json::Value parseInfo() const;

        Json::Value parseHistories() const;

        Json::Value getPlayers() const;

        std::string getPlayRid() const;

        uint64_t getSeed() const;

        bool checkIfPlaying(const int64_t &uid) const;

        bool hasConnection() const;

        bool getStart() const;

        void setStart(const bool &start);

        bool getFinish() const;

        void setFinish(const bool &finish);

        bool checkReady() const;

        bool checkFinished() const;

        Json::Value getDeaths() const;

        uint64_t generatePlace();

    private:
        const std::string _playRid;
        const uint64_t _seed;
        std::unordered_map<uint64_t, bool> _players;
        uint64_t _innerPlace;
        bool _start, _finish;
    };
}