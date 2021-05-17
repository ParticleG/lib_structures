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
                const std::string &password,
                const uint64_t &capacity,
                Json::Value info,
                Json::Value data
        );

        Json::Value getInfo(const std::string &key = "") const;

        void setInfo(const std::string &key, Json::Value value);

        Json::Value getData(const std::string &key = "") const;

        void setData(const std::string &key, Json::Value value);

        bool getPendingStart() const;

        void setPendingStart(const bool &pendingStart);

        bool getStart() const;

        void setStart(const bool &start);

        bool checkReady() const;

        void resetReady();

        bool checkPassword(const std::string &password) const;

        std::string getRelatedStreamRid() const;

        void setRelatedStreamRid(const std::string &relatedStreamRid);

        void publish(const uint64_t &action, Json::Value &&message, const uint64_t &excluded = 0);

        Json::Value getPlayers() const;

        Json::Value getHistory(const unsigned int &begin, const unsigned int &count) const;

        Json::Value parseInfo() const;

    private:
        const unsigned int _maxHistoryCount = 10;
        const std::string _encryptedPassword;
        std::string _relatedStreamRid{};
        Json::Value _info, _data;
        bool _pendingStart, _start;
        std::deque<Json::Value> _history;

        void _setHistory(Json::Value &&data);
    };
}