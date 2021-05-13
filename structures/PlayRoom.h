//
// Created by Parti on 2021/2/27.
//

#include <structures/BaseRoom.h>
#include <structures/Play.h>

namespace tech::structures {
    class PlayRoom : public BaseRoom {
    public:
        explicit PlayRoom(
                std::string id,
                std::string name,
                const std::string &password,
                const uint64_t &capacity,
                Json::Value data
        );

        Json::Value getData(const std::string& key = "") const;

        bool getPendingStart() const;

        void setPendingStart(const bool &pendingStart);

        bool getStart() const;

        void setStart(const bool &start);

        bool checkReady() const;

        void resetReady();

        bool checkPassword(const std::string &password) const;

        void publish(const uint64_t &action, Json::Value &&message);

        void publish(const uint64_t &action, Json::Value &&message, const uint64_t &excluded);

        Json::Value getPlayers() const;

        Json::Value getHistory(const unsigned int &begin, const unsigned int &count) const;

        Json::Value parseInfo() const;

    private:
        const unsigned int _maxHistoryCount = 10;
        const std::string _name, _encryptedPassword;
        Json::Value _data;
        bool _pendingStart, _start;
        std::deque<Json::Value> _history;

        void _setHistory(Json::Value &&data);
    };
}