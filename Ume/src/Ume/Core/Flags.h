#pragma once
#include <stdint.h>
#include <type_traits>


namespace Ume
{
    typedef uint32_t UmeFlags;

    template<typename BitType>
    class Flags {
        static_assert(std::is_enum_v<BitType>, "Flags template parameter must be an enumeration type");
    public:
        using MaskType = std::underlying_type_t<BitType>;

        Flags() : m_Mask(0) {}
        Flags(BitType e) : m_Mask(static_cast<MaskType>(e)) {}

        Flags<BitType>& operator|=(Flags<BitType> e) 
        {
            m_Mask |= static_cast<MaskType>(e);
            return *this;
        }

        Flags<BitType>& operator&=(Flags<BitType> e)
        {
            m_Mask &= static_cast<MaskType>(e);
            return *this;
        }

        Flags<BitType> operator|(Flags<BitType> rhs) const
        {
            return Flags<BitType>((BitType)(m_Mask | rhs.m_Mask));
        }

        Flags<BitType> operator&(Flags<BitType> rhs) const
        {
            return Flags<BitType>((BitType)(m_Mask & rhs.m_Mask));
        }
        
        Flags<BitType> operator^(Flags<BitType> rhs) const
        {
            return Flags<BitType>((BitType)(m_Mask ^ rhs.m_Mask));
        }
        
        Flags<BitType> operator~() const
        {
            return Flags<BitType>((BitType)(~m_Mask));
        }

        bool operator!() const
        {
            return !m_Mask;
        }

        operator MaskType() const
        {
            return m_Mask;
        }

    private:
        MaskType m_Mask;
    };

    template<typename Flag>
    inline Flags<Flag> operator&(Flags<Flag> lhs, Flag rhs)
    {
        using underlying_type = typename std::underlying_type<Flag>::type;
        return Flags<Flag>(static_cast<Flag>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs)));
    }

    template<typename Flag>
    inline Flag operator&(Flag lhs, Flag rhs)
    {
        using underlying_type = typename std::underlying_type<Flag>::type;
        return static_cast<Flag>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs));
    }

    template<typename Flag>
    inline Flag operator|(Flag lhs, Flag rhs)
    {
        using underlying_type = typename std::underlying_type<Flag>::type;
        return static_cast<Flag>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
    }

    template<typename Flag>
    inline Flag operator^(Flag lhs, Flag rhs)
    {
        using underlying_type = typename std::underlying_type<Flag>::type;
        return static_cast<Flag>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs));
    }
}