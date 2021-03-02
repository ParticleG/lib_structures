//
// Created by Parti on 2021/2/27.
//

#include <structures/ChatRoom.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

ChatRoom::ChatRoom(
        string id,
        const uint64_t &capacity
) : BaseRoom(move(id), capacity) {

}

void ChatRoom::publish(const string &message) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair : _connectionsMap) {
        auto connection = pair.second;
        Json::Value response;
        response["message"] = "Broadcast";
        response["action"] = 2;
        response["data"] = _parseInfo(connection, message);
        connection->send(WebSocket::fromJson(response));
    }
}

Json::Value ChatRoom::_parseInfo(const WebSocketConnectionPtr &connection, const string &message) {
    auto info = connection->getContext<Chat>()->getInfo();
    Json::Value result;
    result["rid"] = _id;
    result["uid"] = info->getValueOfId();
    result["username"] = info->getValueOfId();
    result["message"] = message;
    return result;
}


