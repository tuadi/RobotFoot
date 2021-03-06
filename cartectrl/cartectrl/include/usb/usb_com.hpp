/**
  ******************************************************************************
  * @file    usb_com.hpp
  * @author  James-Adam Renquinha Henri (Jarhmander)
  * @version V1.0.0
  * @date    2013-04-10
  * @brief   Allow communication through usb cdc
  ******************************************************************************
  */

//------------------------------------------------------------------------------
#ifndef USB_COM_HPP
#define USB_COM_HPP
//------------------------------------------------------------------------------
#include <stdint.h>
#include <vector>
#include <string>
#include <sys/types.h>

#if __GXX_EXPERIMENTAL_CXX0X__ == 1 /* require c++11 support */
 #include <array>
#endif
//------------------------------------------------------------------------------

/** @addtogroup Peripheral
  * @{
  */

/** @addtogroup USB
  * @{
  */

namespace usb
{

using std::vector;
using std::string;

#if __GXX_EXPERIMENTAL_CXX0X__ == 1 /* require c++11 support */
 using std::array;
#endif


int init();

// Base transmit function
ssize_t write(const uint8_t *m, size_t s);

// Convenience functions:

// All the convenience functions are based on that one. Pass simple pointers to
// object type in it, or you'll get pretty Bad Results™.
// Perfect with pointer to char; okay with pointer to integer, though you should
// mind the endianness of both ends of the communication. You might use a POD
// with no pointer member. Anything else is just plain silly.
template <typename T>
 inline ssize_t write(const T *m, size_t s)
{
    return write(reinterpret_cast<const uint8_t *>(m), s*sizeof(T));
}

// Adaptor: any vector (see comment above for restrictions on type T).
template <typename T>
 inline ssize_t write(const vector<T> &v)
{
    return write(v.data(), v.size());
}
// Adaptor: a string.
inline ssize_t write(const string &s)
{
    return write(s.data(), s.length());
}

// Adaptor: any plain array (see comment above for restrictions on type T).
template <typename T, size_t N>
 inline ssize_t write(const T (&arr)[N])
{
    return write(arr, N);
}

#if __GXX_EXPERIMENTAL_CXX0X__ == 1 /* require c++11 support */

// Adaptor: any std::array (see comment above for restrictions on type T).
template <typename T, size_t N>
 inline ssize_t write(const array<T, N> &arr)
{
    return write(arr.data(), N);
}

#endif

// Base trasmit function
ssize_t read(uint8_t *m, size_t s);

// Convenience functions:

// All the convenience functions are based on that one. Pass simple pointers to
// object type in it, or you'll get pretty Bad Results™, ESPECIALLY more that
// one that the write overload above. The same comment does apply.
template <typename T>
 inline ssize_t read(T *m, size_t s)
{
    return read(reinterpret_cast<uint8_t *>(m), s*sizeof(T));
}

// Adaptor: any vector (see comment above for restrictions on type T).
template <typename T>
 inline ssize_t read(vector<T> &v)
{
    // TODO: Do it better!
    return read(v.data(), v.size());
}
// Adaptor: a string.
inline ssize_t read(string &s)
{
    // TODO: Do it better!
    return read(&*s.begin(), s.length());
}

// Adaptor: any plain array (see comment above for restrictions on type T).
template <typename T, size_t N>
 inline ssize_t read(T (&arr)[N])
{
    return read(arr, N);
}

#if __GXX_EXPERIMENTAL_CXX0X__ == 1 /* require c++11 support */

// Adaptor: any std::array (see comment above for restrictions on type T).
template <typename T, size_t N>
 inline ssize_t read(array<T, N> &arr)
{
    return read(arr.data(), N);
}


unsigned getSOFCount();

bool enabled();

#endif

}

/**
  * @}
  */

/**
  * @}
  */
//------------------------------------------------------------------------------
#endif // USB_COM_HPP

