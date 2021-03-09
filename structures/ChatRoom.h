//
// Created by Parti on 2021/2/27.
//

#include <structures/BaseRoom.h>
#include <structures/Chat.h>

namespace tech::structures {
    class ChatRoom : public BaseRoom {
    public:
        explicit ChatRoom(
                std::string id,
                const uint64_t &capacity,
                const unsigned int &maxHistoryCount
        );

        void publish(Json::Value &&message);

        Json::Value getHistory(const unsigned int &begin, const unsigned int &count) const;

        Json::Value parseInfo() const;

    private:
        const unsigned int _maxHistoryCount;
        std::deque<Json::Value> _history;

        void _setHistory(Json::Value &&data);
    };
}