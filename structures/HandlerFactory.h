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
        virtual std::shared_ptr<baseHandler> createHandler(const unsigned int &action) = 0;

    protected:
        HandlerRegistrarInterface() {}

        virtual ~HandlerRegistrarInterface() {};
    };

    template<class baseHandler>
    class HandlerFactory {
    public:
        static HandlerFactory<baseHandler> &instance() {
            static HandlerFactory<baseHandler> instance;
            return instance;
        };

        void registerHandler(const unsigned int &action, std::shared_ptr<HandlerRegistrarInterface<baseHandler>> registrar) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            _handlerRegistrarsMap[action] = registrar;
        }

        std::shared_ptr<baseHandler> getHandler(const unsigned int &action) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _handlerRegistrarsMap.find(action);
            if (iter != _handlerRegistrarsMap.end()) {
                return iter.second()->createHandler(action);
            }
            throw std::out_of_range("Action not found");
        }

    private:
        HandlerFactory() {};

        ~HandlerFactory() {};

        HandlerFactory(const HandlerFactory &);

        const HandlerFactory &operator=(const HandlerFactory &);

        mutable std::shared_mutex _sharedMutex;

        std::unordered_map<unsigned int, std::shared_ptr<HandlerRegistrarInterface<baseHandler>>> _handlerRegistrarsMap;
    };

    template<class baseHandler, class implementedHandler>
    class HandlerRegistrar : public HandlerRegistrarInterface<baseHandler> {
    public:
        explicit HandlerRegistrar(const unsigned int &action) {
            HandlerFactory<baseHandler>::instance().registerHandler(action, this);
        }

        std::shared_ptr<baseHandler> createHandler(const unsigned int &action) {
            return std::make_shared<implementedHandler>(action);
        }
    };
}