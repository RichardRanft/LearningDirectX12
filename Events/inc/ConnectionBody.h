#pragma once

/*
 *  Copyright(c) 2020 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file ConnectionBody.h
 *  @date March 5, 2020
 *  @author Jeremiah van Oosten
 *
 *  @brief Connection body.
 */

#include "GarbageCollectingLock.h"

#include <cassert>
#include <cstdint>
#include <memory>

class ConnectionBodyBase
{
public:
    ConnectionBodyBase()
    : m_Connected(false)
    , m_SlotRefCount(1)
    {}

    virtual ~ConnectionBodyBase()
    {}

    virtual bool Connected() const = 0;

    void Disconnect()
    {
        GarbageCollectingLock<ConnectionBodyBase> lock(*this);
        NolockDisconnect(lock);
    }

    template<typename Mutex>
    void IncrementSlotRefCount(GarbageCollectingLock<Mutex>& lock) const
    {
        assert(m_SlotRefCount != 0);
        ++m_SlotRefCount;
    }

    template<typename Mutex>
    void DecrementSlotRefCount(GarbageCollectingLock<Mutex>& lock) const
    {
        assert(m_SlotRefCount != 0);
        if (--m_SlotRefCount == 0)
        {
            lock.Push(ReleaseSlot());
        }
    }

    template<typename Mutex>
    void NolockDisconnect(GarbageCollectingLock<Mutex>& lock) const
    {
        if (m_Connected)
        {
            m_Connected = false;
            DecrementSlotRefCount(lock);
        }
    }

    std::shared_ptr<void> GetBlocker()
    {
        std::unique_lock<ConnectionBodyBase> lock(*this);
        std::shared_ptr<void> blocker = m_WeakBlocker.lock();
        if (!blocker)
        {
            // Create a shared pointer to this with a null-deleter (doesn't actually delete this object).
            blocker.reset(this, [](const void*){});
            m_WeakBlocker = blocker;
        }
        return blocker;
    }

    bool Blocked() const
    {
        return !m_WeakBlocker.expired();
    }

    bool NolockNograbConnected() const
    {
        return m_Connected;
    }

    bool NolockNograbBlocked() const
    {
        return (!NolockNograbConnected() || Blocked() );
    }

    // Must implement the lockable concept of a mutex.
    virtual void lock() = 0;
    virtual void unlock() = 0;

protected:
    virtual std::shared_ptr<void> ReleaseSlot() const = 0;

    std::weak_ptr<void> m_WeakBlocker;
private:
    mutable bool m_Connected;
    mutable uint64_t m_SlotRefCount;
};

template<typename GroupKey, typename Slot, typename Mutex>
class ConnectionBody : public ConnectionBodyBase
{
public:
    typedef Slot SlotType;
    typedef Mutex MutexType;

    ConnectionBody(const SlotType& slot, const std::shared_ptr<MutexType>& signalMutex)
    : m_Slot(new SlotType(slot))
    , m_Mutex(signalMutex);
    {}

    virtual ~ConnectionBody()
    {}

    template<typename M, typename OutputIterator>
    void NolockGrabTrackedObjects(GarbageCollectingLock<M>& lock, OutputIterator inserter) const
    {
        if (!m_Slot) return;

    }

    virtual void Connected() const override
    {
        GarbageCollectingLock<MutexType> lock(*m_Mutex);
        
    }

private:
    mutable std::shared_ptr<SlotType> m_Slot;
    mutable std::shared_ptr<MutexType> m_Mutex;
    GroupKey m_GroupKey;
};