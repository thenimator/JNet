#pragma once
#include "../udp.hpp"
#include "../../defines.hpp"
#include "../../Flags/flags.hpp"
#include "../packet.hpp"
#include "BufferManagerDeclaration.hpp"
#include "ReuseableBuffer.hpp"

namespace JNet {
    namespace udp {

        //this somehow isn't recognized as constexpr
        //try separating definition from implementation

        template <uint32_t bufferSize = bufferSize>
        class BufferManagerBase {
        public:
            constexpr static uint32_t getBufferSize() noexcept {
                return bufferSize;
            }
        };



        template <uint32_t bufferSize, safetyFlags flags, bool includeEndpoint>
        class BufferManager : public BufferManagerBase<bufferSize> {
        public:
            typedef ReuseableBuffer<bufferSize, includeEndpoint> ManagedBuffer;
        public:
            BufferManager() = default;
            BufferManager(const BufferManager&) = delete;
            BufferManager& operator=(BufferManager&) = delete;
            ~BufferManager()  {
                for (uint32_t i = 0; i < managedBuffers.size(); i++) {
                    if (debugFlagActive<DebugFlag::bufferManagerDebug>()) {
                        std::stringstream ss;
                        ss << "Deleting buffer with id: " << managedBuffers[i] <<"\n";
                        std::cout << ss.str();
                    }
                        
                    delete managedBuffers[i];
                }
            }

            void recycleBuffer(ManagedBuffer* buffer) {
                std::unique_lock firstLock(firstMutex,std::defer_lock);
                std::unique_lock lastLock(lastMutex,std::defer_lock);
                //std::cout << firstLock.owns_lock() << "\n";
                //std::unique_lock firstLock(lastMutex);
                //std::unique_lock lastLock(lastMutex);
                if (debugFlagActive<DebugFlag::bufferManagerDebug>())  {
                    std::stringstream ss; 
                    ss << "Recycling buffer with id: " << buffer << "\n";
                    std::cout << ss.str();
                }
                    

                if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                    //lastLock.lock();
                    std::lock(lastLock,firstLock);

                if (last == nullptr) {
                    if (hasFlag<SafetyFlag::threadSafe>(flags)) {
                        //lastLock.unlock();
                        //std::lock(firstLock,lastLock);
                    } 
                    first = buffer;
                    last = buffer;
                    return;
                }
                last->next = buffer;
                last = buffer;


                

            }

            ManagedBuffer* getBuffer() {
                std::unique_lock firstLock(firstMutex,std::defer_lock);
                std::unique_lock lastLock(lastMutex,std::defer_lock);     
                if (hasFlag<SafetyFlag::threadSafe>(flags))  
                    std::lock(firstLock,lastLock);

                if (first == nullptr) {
                    ManagedBuffer* buffer = new ManagedBuffer;
                    managedBuffers.push_back(buffer);
                    if (debugFlagActive<DebugFlag::bufferManagerDebug>())  {
                        std::stringstream ss;
                        ss << "returning new buffer with id: " << buffer <<"\n";
                        std::cout << ss.str();
                    }
                    return buffer;
                }
                ManagedBuffer* returnBuffer = first;
                if (debugFlagActive<DebugFlag::bufferManagerDebug>()) {
                    std::stringstream ss;
                    ss << "Returning existing buffer with id: " << returnBuffer <<"\n";
                    std::cout << ss.str();
                }
                    
                if (hasFlag<SafetyFlag::threadSafe>(flags)) {
                    //firstLock.unlock();
                    //std::lock(firstLock,lastLock);
                    //lastLock.lock();
                }
                    
                if (first == last) {
                    first = nullptr;
                    //if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                        //firstLock.unlock();
                    last = nullptr;
                    
                    return returnBuffer;
                }
                //if (hasFlag<SafetyFlag::threadSafe>(flags)) 
                    //lastLock.unlock();

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