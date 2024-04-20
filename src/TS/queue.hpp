//Emplace should be added to allow for performance improvements
#pragma once
#include <queue>
#include <mutex>


namespace JNet {
    namespace ts {

        template<class T>
        class Queue {
        public:
            T& front() {
                std::scoped_lock lock(dataMutex);
                return data.front();
            }
            const T& front() const {
                std::scoped_lock lock(dataMutex);
                return data.front();
            }
            void push(const T& insertElement) {
                std::scoped_lock lock(dataMutex);
                data.push(insertElement);
            }
            void push(T&& insertElement) {
                std::scoped_lock lock(dataMutex);
                data.push(insertElement);
            }
            void pop() {
                std::scoped_lock lock(dataMutex);
                data.pop();
            }
        private:
            std::queue<T> data;
            std::mutex dataMutex;
        };



    }
}