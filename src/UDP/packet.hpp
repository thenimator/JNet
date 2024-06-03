#pragma once
#include "../Messages/message.hpp"
#include "../Flags/flags.hpp"
#include "../Exceptions/exceptions.hpp"

namespace JNet {
    namespace udp {
        template <uint32_t TBufferSize = 0x10000, safetyFlags flags = SafetyFlag::unSafe>
        class Packet {
            static_assert(flags == SafetyFlag::unSafe || flags == SafetyFlag::runtimeBoundsChecks, "Given flags aren't supported");
            static_assert(TBufferSize <= 0x10000, "TBufferSize is greater than max udp packet size. Data loss is guaranteed");
            static_assert(TBufferSize >= sizeof(Header), "TBufferSize must be greater than sizeof(Header)");
            //friend std::ostream& operator<< (std::ostream& os, const Packet<TBufferSize, flags>& packet);
        public:
            Packet();
            uint64_t getId() const; 
            void setId(uint64_t id);
            /** @brief Get the size of data inside the packet
             * @exception Size returned can be greater than actual size (although not out of bounds) if the size was set up incorrectly or in case of a malicious attack. In this case either the max size is returned or an exception thrown
             * @throws throws an exception if size is greater than buffer if runtimeBoundsChecks are enabled
             * @return size stored in the header of the packet 
             */
            size_t getSize() const;
            void setSize(size_t size);
            MessageType getMessageType();
            void setMessageType(MessageType type);
            void* getData();
            const std::array<uint8_t,TBufferSize>& getBuffer() const;
        private:
            /** @brief The first sizeof(Header) bytes are used to store the header of the udp packet. All remaining bytes are used to store data
             */
            std::array<uint8_t,TBufferSize> buffer;
        };



        template <uint32_t TBufferSize, safetyFlags flags>
        const std::array<uint8_t,TBufferSize>& Packet<TBufferSize,flags>::getBuffer() const {
            return buffer;
        }

        template <uint32_t TBufferSize, safetyFlags flags>
        MessageType Packet<TBufferSize,flags>::getMessageType() {
            return reinterpret_cast<Header&>(buffer).messageType;
        }
        template <uint32_t TBufferSize, safetyFlags flags>
        void Packet<TBufferSize,flags>::setMessageType(MessageType type) {
            reinterpret_cast<Header&>(buffer).messageType = type;
        }

        template <uint32_t TBufferSize, safetyFlags flags>
        void* Packet<TBufferSize,flags>::getData() {
            return &buffer[sizeof(Header)];
        }

        template <uint32_t TBufferSize, safetyFlags flags>
        void Packet<TBufferSize,flags>::setId(uint64_t id) {
            reinterpret_cast<Header&>(buffer).id = id;
        }

        template <uint32_t TBufferSize, safetyFlags flags>
        uint64_t Packet<TBufferSize,flags>::getId() const {
            return reinterpret_cast<const Header&>(buffer).id;
        }

        /*template <uint32_t TBufferSize, safetyFlags flags>
        std::ostream& operator << (std::ostream& os, const Packet<TBufferSize,flags>& packet) {
            os << "ID: " << packet.getId() << " Size: " << packet.getSize();
            return os;
        }*/

        template <uint32_t TBufferSize, safetyFlags flags>
        Packet<TBufferSize, flags>::Packet() {
            reinterpret_cast<Header&>(buffer).id = 0;
            reinterpret_cast<Header&>(buffer).size = 0;
            reinterpret_cast<Header&>(buffer).messageType = MessageType::Unset;
        }

        template <uint32_t TBufferSize, safetyFlags flags>
        size_t Packet<TBufferSize, flags>::getSize() const {
            if (reinterpret_cast<const Header&>(buffer).size  <= (TBufferSize - sizeof(Header)))
                return reinterpret_cast<const Header&>(buffer).size;
            if (hasFlag<SafetyFlag::runtimeBoundsChecks>(flags))
                throw JNet::BadPacketError("Stored size is greater than buffer size");
            return TBufferSize - sizeof(Header);
        }
        
        template <uint32_t TBufferSize, safetyFlags flags>
        void Packet<TBufferSize, flags>::setSize(size_t size) {
            reinterpret_cast<Header&>(buffer).size = size;
        }

    }
}