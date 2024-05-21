//Emplace should be added to allow for performance improvements
#pragma once
#include <deque>
#include <mutex>
#include <memory>


namespace JNet {
    namespace ts {
        template<class T>
        class Queue {
        public:
            Queue() = default;
            Queue(const Queue&) = delete;
            Queue& operator=(const Queue&) = delete;

            T&& consumeFront() {
                std::scoped_lock lock(dataMutex);
                T temp = std::move(data.front());
                data.pop_front();
                return std::move(temp);
            }

            bool empty() {
                std::scoped_lock lock(dataMutex);
                return data.empty();
            }

            T& front() {
                std::scoped_lock lock(dataMutex);
                return data.front();
            }
            
            const T& front() const {
                std::scoped_lock lock(dataMutex);
                return data.front();
            }
            
            void push(T&& insertElement) {
                std::scoped_lock lock(dataMutex);
                data.push_back(insertElement);
            }
            
            void pop() {
                std::scoped_lock lock(dataMutex);
                data.pop_front();
            }
        private:
            std::deque<T> data;
            std::mutex dataMutex;
        };



        



    }
}