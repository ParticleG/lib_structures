//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <shared_mutex>

namespace tech::structures {
    template<class baseHandler>
    class HandlerRegistrarInterface {
    public:
        virtual baseHandler *createHandler() = 0;

    protected:
        HandlerRegistrarInterface() {}

        virtual ~HandlerRegistrarInterface() {};
    };

    template<class baseHandler>
    class HandlerFactory {
    public:
        HandlerFactory(const HandlerFactory &) = delete;

        const HandlerFactory &operator=(const HandlerFactory &) = delete;

        static HandlerFactory<baseHandler> &instance() {
            static HandlerFactory<baseHandler> instance;
            return instance;
        };

        void registerHandler(const unsigned int &action, HandlerRegistrarInterface<baseHandler> *registrar) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            _handlerRegistrarsMap[action] = registrar;
        }

        baseHandler *getHandler(const unsigned int &action) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _handlerRegistrarsMap.find(action);
            if (iter != _handlerRegistrarsMap.end()) {
                return iter->second->createHandler();
            }
            throw std::out_of_range("Action not found");
        }

    private:
        HandlerFactory() {}

        ~HandlerFactory() {}

        mutable std::shared_mutex _sharedMutex;

        std::unordered_map<unsigned int, HandlerRegistrarInterface<baseHandler> *> _handlerRegistrarsMap;
    };

    template<class baseHandler, class implementedHandler>
    class HandlerRegistrar : public HandlerRegistrarInterface<baseHandler> {
    public:
        explicit HandlerRegistrar(const unsigned int &action) {
            HandlerFactory<baseHandler>::instance().registerHandler(
                    action,
                    this
            );
        }

        baseHandler *createHandler() {
            return new implementedHandler();
        }
    };
}