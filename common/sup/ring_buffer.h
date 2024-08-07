#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <cstdint>
#include <cstddef>
#include <algorithm> // for std::copy

template <typename T, std::size_t S>
class RingBuffer
{
public:
    RingBuffer();
    bool push(const T &data);
    bool push(const T *data, std::size_t len);
    T pop();
    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;
    bool full() const;
    const T *front() const;
    std::size_t chunkSize() const;
    bool remove(std::size_t len);

private:
    mutable T mBuffer[S];
    mutable T *mInPtr;
    mutable T *mOutPtr;
};

template <typename T, std::size_t S>
RingBuffer<T, S>::RingBuffer() : mBuffer{}, mInPtr(mBuffer), mOutPtr(mBuffer) {}

template <typename T, std::size_t S>
bool RingBuffer<T, S>::push(const T &data)
{
    if (full())
    {
        return false;
    }
    *mInPtr = data;
    mInPtr = mBuffer + ((mInPtr - mBuffer + 1) % S);
    return true;
}

template <typename T, std::size_t S>
bool RingBuffer<T, S>::push(const T *data, std::size_t len)
{
    if (len > capacity() - size())
    {
        return false;
    }
    std::size_t spaceToEnd = mBuffer + S - mInPtr;
    if (len <= spaceToEnd)
    {
        std::copy(data, data + len, mInPtr);
    }
    else
    {
        std::copy(data, data + spaceToEnd, mInPtr);
        std::copy(data + spaceToEnd, data + len, mBuffer);
    }
    mInPtr = mBuffer + ((mInPtr - mBuffer + len) % S);
    return true;
}

template <typename T, std::size_t S>
T RingBuffer<T, S>::pop()
{
    if (empty())
    {
        return T();
    }
    T &result = *mOutPtr;
    mOutPtr = mBuffer + ((mOutPtr - mBuffer + 1) % S);
    return result;
}

template <typename T, std::size_t S>
std::size_t RingBuffer<T, S>::size() const
{
    if (mInPtr >= mOutPtr)
    {
        return mInPtr - mOutPtr;
    }
    else
    {
        return S - (mOutPtr - mInPtr);
    }
}

template <typename T, std::size_t S>
std::size_t RingBuffer<T, S>::capacity() const
{
    return S;
}

template <typename T, std::size_t S>
bool RingBuffer<T, S>::empty() const
{
    if(mInPtr == mOutPtr) {
        mInPtr = mBuffer;
        mOutPtr = mBuffer;
    }
    return mInPtr == mOutPtr;
}

template <typename T, std::size_t S>
bool RingBuffer<T, S>::full() const
{
    return size() == capacity() - 1;
}

template <typename T, std::size_t S>
const T *RingBuffer<T, S>::front() const
{
    return mOutPtr;
}

template <typename T, std::size_t S>
std::size_t RingBuffer<T, S>::chunkSize() const
{
    return std::min(size(), static_cast<size_t>(mBuffer + S - front()));
}

template <typename T, std::size_t S>
bool RingBuffer<T, S>::remove(std::size_t len)
{
    if(len>size()) {
        return false;
    }
    mOutPtr = mBuffer + ((mOutPtr - mBuffer + len) % S);
    return true;
}
#endif