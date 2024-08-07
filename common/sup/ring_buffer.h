#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <cstdint>
#include <cstddef>
#include <algorithm> // for std::copy

/// @brief A template class implementing a ring buffer.
///
/// @tparam T The type of elements stored in the ring buffer.
/// @tparam S The capacity of the ring buffer.
template <typename T, std::size_t S>
class RingBuffer
{
public:
    /// @brief Construct a new RingBuffer object.
    RingBuffer();

    /// @brief Push a single element into the ring buffer.
    ///
    /// @param data The element to push into the ring buffer.
    /// @return true If the element was successfully pushed.
    /// @return false If the ring buffer is full.
    bool push(const T &data);

    /// @brief Push multiple elements into the ring buffer.
    ///
    /// @param data Pointer to the elements to push into the ring buffer.
    /// @param len The number of elements to push.
    /// @return true If the elements were successfully pushed.
    /// @return false If there is not enough space in the ring buffer.
    bool push(const T *data, std::size_t len);

    /// @brief Pop a single element from the ring buffer.
    ///
    /// @return T The popped element.
    T pop();

    /// @brief Get the number of elements currently stored in the ring buffer.
    ///
    /// @return std::size_t The number of elements in the ring buffer.
    std::size_t size() const;

    /// @brief Get the capacity of the ring buffer.
    ///
    /// @return std::size_t The capacity of the ring buffer.
    std::size_t capacity() const;

    /// @brief Check if the ring buffer is empty.
    ///
    /// @return true If the ring buffer is empty.
    /// @return false If the ring buffer is not empty.
    bool empty() const;

    /// @brief Check if the ring buffer is full.
    ///
    /// @return true If the ring buffer is full.
    /// @return false If the ring buffer is not full.
    bool full() const;

    /// @brief Get a pointer to the front element in the ring buffer.
    ///
    /// @return const T* Pointer to the front element.
    const T *front() const;

    /// @brief Get the size of the continuous chunk of elements starting from the front.
    ///
    /// @return std::size_t The size of the continuous chunk.
    std::size_t chunkSize() const;

    /// @brief Remove a specified number of elements from the front of the ring buffer.
    ///
    /// @param len The number of elements to remove.
    /// @return true If the elements were successfully removed.
    /// @return false If the specified number of elements exceeds the number of elements in the ring buffer.
    bool remove(std::size_t len);

private:
    T mBuffer[S]; ///< The buffer storing the elements.
    T *mInPtr;    ///< Pointer to the position where the next element will be inserted.
    T *mOutPtr;   ///< Pointer to the position of the next element to be removed.
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
    if (len > size())
    {
        return false;
    }
    mOutPtr = mBuffer + ((mOutPtr - mBuffer + len) % S);
    return true;
}

#endif
