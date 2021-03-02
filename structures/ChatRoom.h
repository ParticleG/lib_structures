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
                const uint64_t &capacity
        );

        void publish(const std::string &message);

    private:
        const static unsigned int _maxRecentMessages = 10;
        std::deque<std::string> _recentMessages;

        Json::Value _parseInfo(
                const drogon::WebSocketConnectionPtr &connection,
                const std::string &message
        );
    };
}