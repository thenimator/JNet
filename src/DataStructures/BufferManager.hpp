#pragma once
#include "../defines.hpp"
#include <iterator>
#include "../Flags/flags.hpp"

namespace JNet {
    namespace data {
        template <uint32_t bufferSize = 0x10000, safetyFlags flags = 0>
        class BufferManager;
        

        template <const uint32_t bufferSize = 0x10000>
        class ReuseableBuffer {
        public:
            friend class BufferManager<bufferSize>;
            std::array<uint8_t, bufferSize> buffer;
        private:
            ReuseableBuffer* next;

            
        };

        //this somehow isn't recognized as constexpr
        template <uint32_t bufferSize = 0x10000>
        class BufferManagerBase {
        public:
            constexpr static uint32_t getBufferSize() noexcept {
                return bufferSize;
            }
        };

        template <uint32_t bufferSize, safetyFlags flags>
        class BufferManager : public BufferManagerBase<bufferSize> {
        public:
            typedef ReuseableBuffer<bufferSize> ManagedBuffer;
        public:
            BufferManager() = default;
            BufferManager(const BufferManager&) = delete;
            BufferManager& operator=(BufferManager&) = delete;
            ~BufferManager()  {
                for (uint32_t i = 0; i < managedBuffers.size(); i++) {
                    delete managedBuffers[i];
                }
            }
            void recycleBuffer(ManagedBuffer* buffer) {
                std::unique_lock firstLock(lastMutex,std::defer_lock);
                std::unique_lock lastLock(lastMutex,std::defer_lock);

                if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                    lastLock.lock();

                if (last == nullptr) {
                    if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                        firstLock.lock();
                    
                    first = buffer;
                    last = buffer;
                    

                    return;
                }
                last->next = buffer;
                last = buffer;


                

            }

            ReuseableBuffer<bufferSize>* getBuffer() {
                std::unique_lock firstLock(lastMutex,std::defer_lock);
                std::unique_lock lastLock(lastMutex,std::defer_lock);
                if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                        firstLock.lock();
            
                if (first == nullptr) {
                    return new ManagedBuffer;
                }
                ManagedBuffer* returnBuffer = first;
                if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                    lastLock.lock();
                if (first == last) {
                    first = nullptr;
                    firstLock.unlock();
                    last = nullptr;
                    return returnBuffer;
                }
                if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                    lastLock.unlock();

                first = first->next;
                return returnBuffer;
                
                
            }


        private:
            ManagedBuffer* first = nullptr;
            ManagedBuffer* last = nullptr;
            std::mutex firstMutex;
            std::mutex lastMutex;

            std::vector<ManagedBuffer*> managedBuffers;

        };
    }
    



}