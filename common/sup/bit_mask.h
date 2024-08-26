/// @brief A template class for managing a bitmask of flags.
///
/// This class provides methods to set, clear, and check individual bits in a bitmask
/// using a generic enumeration or integer type.
///
/// @tparam T The type used to specify individual bits (typically an enumeration).
template <typename T>
class BitMask {
public:
    /// @brief Construct a new BitMask object with all bits cleared.
    BitMask() : mData(0) {}

    /// @brief Set the specified bits in the bitmask.
    ///
    /// This method sets the bits in the bitmask corresponding to the specified data.
    ///
    /// @param data The bits to set (typically an enumeration value).
    void set(const T& data) {
        mData |= static_cast<uint32_t>(data);
    }

    /// @brief Clear the specified bits in the bitmask.
    ///
    /// This method clears the bits in the bitmask corresponding to the specified data.
    ///
    /// @param data The bits to clear (typically an enumeration value).
    void clr(const T& data) {
        mData &= ~static_cast<uint32_t>(data);
    }

    /// @brief Check if the specified bits are set in the bitmask.
    ///
    /// This method returns true if the bits corresponding to the specified data are set.
    ///
    /// @param data The bits to check (typically an enumeration value).
    /// @return true if the specified bits are set, false otherwise.
    bool isSet(const T& data) const {
        return mData & static_cast<uint32_t>(data);
    }

private:
    uint32_t mData; ///< The bitmask data.
};