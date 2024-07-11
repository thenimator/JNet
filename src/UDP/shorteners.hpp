#pragma once
#include "udp.hpp"
#include "Buffer/BufferManager.hpp"
#include "ReuseablePacket.hpp"

#define UDPTYPES ReuseableBuffer = udp::ReuseableBuffer<JNet::udp::bufferSize,true>; using BufferManager = udp::BufferManager<JNet::udp::bufferSize, SafetyFlag::threadSafe, true>; using ReuseablePacket = JNet::udp::ReuseablePacket<TPacketWrapper ,JNet::udp::bufferSize, true>;
