///
/// BSD 3-Clause License
///
/// Copyright (c) 2022, Alexandre Arsenault
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// * Redistributions of source code must retain the above copyright notice, this
///   list of conditions and the following disclaimer.
///
/// * Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// * Neither the name of the copyright holder nor the names of its
///   contributors may be used to endorse or promote products derived from
///   this software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#pragma once

#include <array>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <string>
#include <string_view>

#pragma clang diagnostic push

#pragma clang diagnostic warning "-Weverything"
#pragma clang diagnostic ignored "-Wc++98-compat"

#ifdef _MSVC_LANG
  #define UNICODE_CPP_VERSION _MSVC_LANG
#elif defined(__cplusplus)
  #define UNICODE_CPP_VERSION __cplusplus
#else
  #warning Unknown C++ version
#endif

#if UNICODE_CPP_VERSION >= 202002L
  #define UNICODE_CPP_20

namespace utf {
template <class T>
using remove_cvref_t = std::remove_cvref_t<T>;
} // namespace utf.

#elif UNICODE_CPP_VERSION >= 201703L

namespace utf {
template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
} // namespace utf.

  #ifdef UNICODE_DECLARE_CHAR8_T
using char8_t = char;
namespace std {
using u8string = std::string;
using u8string_view = std::string_view;
} // namespace std
  #endif // UNICODE_DECLARE_CHAR8_T.
#else
  #error C++ version not supported
#endif

namespace utf {

///
///
///
enum class encoding { utf8 = sizeof(char), utf16 = sizeof(char16_t), utf32 = sizeof(char32_t) };

///
///
///
template <typename CharT>
struct is_char_type;

///
///
///
template <class SType>
struct is_string_type;

///
///
///
template <std::size_t CharSize>
struct is_char_size;

///
///
///
template <encoding Encoding>
struct encoding_size;

///
///
///
template <encoding Encoding>
struct encoding_to_max_char_count;

///
///
///
template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t> = nullptr>
struct encoding_of;

///
///
///
template <class SType, typename = void>
struct string_char_type;

///
///
///
template <class SType>
using string_char_type_t = typename string_char_type<SType>::type;

///
///
///
template <typename CharT, class SType,
    std::enable_if_t<is_string_type<SType>::value && is_char_type<CharT>::value, std::nullptr_t> = nullptr>
inline std::size_t convert_size(const SType& str);

///
///
///
template <typename CharT, class SType,
    std::enable_if_t<is_string_type<SType>::value && is_char_type<CharT>::value, std::nullptr_t> = nullptr>
inline std::basic_string<CharT> convert_as(const SType& str);

///
///
///
template <class SType, class OutputIt, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t> = nullptr>
inline OutputIt copy(const SType& str, OutputIt outputIt);

///
///
///
template <class SType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t> = nullptr>
inline std::size_t length(const SType& str);

///
///
///
template <class SType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t> = nullptr>
class convert;

///
///
///
template <typename CharT, typename SType>
inline auto iterate_as(const SType& str);

///
///
///
template <typename SType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t> = nullptr>
inline auto iterate(const SType& str);

///
///
///
template <class IteratorType>
class iterator;

///
///
///
template <class CharT, class SType, class IteratorType,
    std::enable_if_t<is_string_type<SType>::value, std::nullptr_t> = nullptr>
class basic_iterator;

// Forward declared.
class string_view;

/// Converts any string type to a utf8 std::string.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
inline std::string to_utf8(string_view s);

/// Converts any string type to a utf16 std::u16string.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
inline std::u16string to_utf16(string_view s);

/// Converts any string type to a utf32 std::u32string.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
inline std::u32string to_utf32(string_view s);

/// Converts any string type to a std::wstring.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
inline std::wstring to_wide(string_view s);

/*!
 * @brief   Generalization of a std::basic_string_view that accepts any char type.
 *
 * @details The mts::unicode::string_view describes an object that can refer to a constant
 *          contiguous sequence of any char-like objects with the first element of
 *          the sequence at position zero.
 *
 *          This class is intended to be used when you need to support multiple
 *          character types and don't want to write a function for each of them.
 *
 *          It also prevents from creating unnecessary std::basic_string
 *          when passing a string literals or a std::basic_string_view to a function.
 *
 * @warning Keep in mind that the size of wchar_t is 2 bytes on Windows and 4 bytes
 *          on other platforms.
 *
 * Here's an example on how this could be used for drawing multiple string types (drawText)
 * or how the string_view could be converted to any string type and help to abstract a
 * cross-platform implementation (setText) :
 *
 * @code
 *   // In example.h
 *   void drawText(mts::unicode::string_view text);
 *   void setText(mts::unicode::string_view text);
 *
 *   // In example.cpp
 *   void drawText(mts::unicode::string_view text) {
 *     switch(text.encoding()) {
 *       case mts::unicode::string_view::UTF8:
 *         drawUtf8Text(text.u8view());
 *         break;
 *       case mts::unicode::string_view::UTF16:
 *         drawUtf16Text(text.u16view());
 *         break;
 *       case mts::unicode::string_view::UTF32:
 *         drawUtf32Text(text.u32view());
 *         break;
 *     }
 *   }
 *
 *   #ifdef _WIN32
 *   void setTextWindows(const std::wstring&) { ... }
 *   #else
 *   void setTextMac(const std::string&) { ... }
 *   #endif
 *
 *   void setText(mts::unicode::string_view text) {
 *     #ifdef _WIN32
 *     setTextWindows(text.to_wide());
 *     #else
 *     setTextMac(text.to_utf8());
 *     #endif
 *   }
 *
 *   // In main.cpp
 *   #include "example.h"
 *
 *   int main() {
 *     std::string text = "Text";
 *     std::wstring wtext = L"Text";
 *
 *     drawText(text);
 *     drawText(wtext);
 *     drawText("Text");
 *     drawText(L"Text");
 *
 *     setText(text);
 *     setText(wtext);
 *     setText("Text");
 *     setText(L"Text");
 *     return 0;
 *   }
 * @endcode
 */
class string_view {
public:
  /// Default constructor.
  /// Creates an empty string view of char type (UTF8).
  inline string_view() noexcept;

  /// Constructor.
  /// Takes any possible kind of string input:
  ///
  /// Strings:
  /// const std::string&
  /// const std::u16string&
  /// const std::u32string&
  /// const std::wstring&
  ///
  /// Views:
  /// std::string_view
  /// std::u16string_view
  /// std::u32string_view
  /// std::wstring_view
  ///
  /// Pointers:
  /// const char*
  /// const char16_t*
  /// const char32_t*
  /// const wchar_t*
  ///
  /// Literals:
  /// const char(&)[N]
  /// const char16_t(&)[N]
  /// const char32_t(&)[N]
  /// const wchar_t(&)[N]
  template <typename T, std::enable_if_t<is_string_type<remove_cvref_t<T>>::value, std::nullptr_t> = nullptr>
  inline string_view(T&& s);

  string_view(string_view&&) noexcept = default;
  string_view(const string_view&) noexcept = default;

  string_view& operator=(string_view&&) noexcept = default;
  string_view& operator=(const string_view&) noexcept = default;

  /// Indicates if the character array is null terminated.
  inline bool null_terminated() const noexcept { return m_nullTerminated; }

  /// Returns true if the character array contains no characters.
  inline bool empty() const noexcept { return !m_size; }

  /// Returns the number of characters.
  inline std::size_t size() const noexcept { return static_cast<std::size_t>(m_size); }

  /// Returns the size of a single char (i.e. sizeof(char_type)).
  inline std::size_t char_size() const noexcept { return static_cast<std::size_t>(m_charSize); }

  /// Returns the size of the string in bytes (i.e. size() * char_size()).
  inline std::size_t size_bytes() const noexcept { return m_size * char_size(); }

  /// Returns the human readable character count.
  inline std::size_t count() const;

  /// Returns the encoding.
  inline encoding encoding() const noexcept;

  /// Returns a pointer to a null-terminated character array with data equivalent to those
  /// stored in the string. The pointer is such that the range [c_str(); c_str() + size()]
  /// is valid and the values in it correspond to the values stored in the string with an
  /// additional null character after the last position.
  ///
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          A nullptr is returned if sizeof(CharT) != char_size().
  ///
  /// @warning This can only be called when null_terminated() returns true.
  ///          A nullptr is returned if null_terminated() == false.
  ///
  /// @see     data() or view() for non null-terminated strings.
  template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t> = nullptr>
  inline const CharT* c_str() const noexcept;

  /// Same as c_str<char>().
  inline const char* u8cstr() const noexcept;

  /// Same as c_str<char16_t>().
  inline const char16_t* u16cstr() const noexcept;

  /// Same as c_str<char32_t>().
  inline const char32_t* u32cstr() const noexcept;

  /// Same as c_str<wchar_t>().
  inline const wchar_t* wcstr() const noexcept;

  /// Returns a pointer to the character array.
  /// The pointer is such that the range [data(); data() + size()) is valid and the values
  /// in it correspond to the values of the view.
  ///
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          A nullptr is returned if sizeof(CharT) != char_size().
  ///
  /// @warning Unlike std::basic_string::data() and string literals, data() may return a pointer
  ///          to a buffer that is not null-terminated. Therefore it is typically a mistake to
  ///          pass data() to a routine that takes just a const CharT* and expects a
  ///          null-terminated string.
  ///
  /// @see     null_terminated()
  template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t> = nullptr>
  inline const CharT* data() const noexcept;

  /// Same as data<char>().
  inline const char* u8data() const noexcept;

  /// Same as data<char16_t>().
  inline const char16_t* u16data() const noexcept;

  /// Same as data<char32_t>().
  inline const char32_t* u32data() const noexcept;

  /// Same as data<wchar_t>().
  inline const wchar_t* wdata() const noexcept;

  /// Returns a std::basic_string<CharT>.
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          An empty std::basic_string<CharT> is returned if sizeof(CharT) != char_size().
  template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t> = nullptr>
  inline std::basic_string<CharT> str() const;

  /// Same as str<char>()
  inline std::string u8str() const noexcept;

  /// Same as str<char16_t>()
  inline std::basic_string<char16_t> u16str() const noexcept;

  /// Same as str<char32_t>()
  inline std::basic_string<char32_t> u32str() const noexcept;

  /// Same as str<wchar_t>()
  inline std::wstring wstr() const noexcept;

  /// Returns a std::basic_string_view<CharT>.
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          An empty std::basic_string_view<CharT> is returned if sizeof(CharT) != char_size().
  template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t> = nullptr>
  inline std::basic_string_view<CharT> view() const;

  /// Same as view<char>().
  inline std::string_view u8view() const noexcept;

  /// Same as view<char16_t>().
  inline std::basic_string_view<char16_t> u16view() const noexcept;

  /// Same as view<char32_t>().
  inline std::basic_string_view<char32_t> u32view() const noexcept;

  /// Same as view<wchar_t>().
  inline std::wstring_view wview() const noexcept;

  /// Converts the character array to a utf8 std::string.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  inline std::string to_utf8() const;

  /// Converts the character array to a utf16 std::u16string.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  inline std::u16string to_utf16() const;

  /// Converts the character array to a utf32 std::u32string.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  inline std::u32string to_utf32() const;

  /// Converts the character array to a std::wstring.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  inline std::wstring to_wide() const;

private:
  union content {
    inline content() noexcept;
    inline content(const char* s) noexcept;
    inline content(const char16_t* s) noexcept;
    inline content(const char32_t* s) noexcept;
    inline content(const wchar_t* s) noexcept;

    inline const wchar_t* cw() const noexcept;

    const char* c8;
    const char16_t* c16;
    const char32_t* c32;
  } m_data;

  std::uint32_t m_size;
  int m_charSize;
  bool m_nullTerminated;

  struct normal_tag {};

  struct null_terminated_tag {};

  template <typename T>
  inline static auto make(T&& s);

  inline string_view(const std::string& str, normal_tag) noexcept;
  inline string_view(const std::u16string& str, normal_tag) noexcept;
  inline string_view(const std::u32string& str, normal_tag) noexcept;
  inline string_view(const std::wstring& str, normal_tag) noexcept;
  inline string_view(std::string_view str, normal_tag) noexcept;
  inline string_view(std::u16string_view str, normal_tag) noexcept;
  inline string_view(std::u32string_view str, normal_tag) noexcept;
  inline string_view(std::wstring_view str, normal_tag) noexcept;
  inline string_view(std::string_view str, null_terminated_tag) noexcept;
  inline string_view(std::u16string_view str, null_terminated_tag) noexcept;
  inline string_view(std::u32string_view str, null_terminated_tag) noexcept;
  inline string_view(std::wstring_view str, null_terminated_tag) noexcept;
};

//
//
//**********************************************************************************************
// Implementation.
//**********************************************************************************************
//
//
//

template <typename CharT>
struct is_char_type : std::bool_constant<std::is_same<char, std::remove_cv_t<CharT>>::value //
                          || std::is_same<char16_t, std::remove_cv_t<CharT>>::value //
                          || std::is_same<char32_t, std::remove_cv_t<CharT>>::value //
                          || std::is_same<wchar_t, std::remove_cv_t<CharT>>::value //
#ifdef UNICODE_CPP_20
                          || std::is_same<char8_t, std::remove_cv_t<CharT>>::value
#endif
                          > {
};

template <class SType>
struct is_string_type : std::bool_constant<std::is_constructible<std::string_view, SType>::value //
                            || std::is_constructible<std::u16string_view, SType>::value //
                            || std::is_constructible<std::u32string_view, SType>::value //
                            || std::is_constructible<std::wstring_view, SType>::value //
#ifdef UNICODE_CPP_20
                            || std::is_constructible<std::u8string_view, SType>::value
#endif
                            > {
};

template <std::size_t CharSize>
struct is_char_size {
  static constexpr bool value
      = CharSize == sizeof(char) || CharSize == sizeof(char16_t) || CharSize == sizeof(char32_t);
};

template <encoding Encoding>
struct encoding_size {
  static constexpr std::size_t value = static_cast<std::size_t>(Encoding);
};

template <encoding Encoding>
struct encoding_to_max_char_count {
  static constexpr std::size_t value = []() {
    if constexpr (Encoding == encoding::utf8) {
      return 4;
    }
    else if constexpr (Encoding == encoding::utf16) {
      return 2;
    }
    else if constexpr (Encoding == encoding::utf32) {
      return 1;
    }
  }();
};

template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t>>
struct encoding_of {
  static constexpr encoding value = []() {
    if constexpr (sizeof(CharT) == sizeof(char)) {
      return encoding::utf8;
    }
    else if constexpr (sizeof(CharT) == sizeof(char16_t)) {
      return encoding::utf16;
    }
    else if constexpr (sizeof(CharT) == sizeof(char32_t)) {
      return encoding::utf32;
    }
  }();
};

template <class SType, typename>
struct string_char_type {
  using type = void;
};

template <class SType>
struct string_char_type<SType,
    std::enable_if_t<std::is_constructible<std::basic_string_view<char>, utf::remove_cvref_t<SType>>::value>> {
  using type = char;
};

#ifdef UNICODE_CPP_20
template <class SType>
struct string_char_type<SType,
    std::enable_if_t<std::is_constructible<std::basic_string_view<char8_t>, unicode::remove_cvref_t<SType>>::value>> {
  using type = char8_t;
};
#endif // UNICODE_CPP_20

template <class SType>
struct string_char_type<SType,
    std::enable_if_t<std::is_constructible<std::basic_string_view<char16_t>, utf::remove_cvref_t<SType>>::value>> {
  using type = char16_t;
};

template <class SType>
struct string_char_type<SType,
    std::enable_if_t<std::is_constructible<std::basic_string_view<char32_t>, utf::remove_cvref_t<SType>>::value>> {
  using type = char32_t;
};

template <class SType>
struct string_char_type<SType,
    std::enable_if_t<std::is_constructible<std::basic_string_view<wchar_t>, utf::remove_cvref_t<SType>>::value>> {
  using type = wchar_t;
};

namespace detail {
  template <class T>
  struct output_iterator_traits : std::iterator_traits<T> {};

  template <class OutputIt, class T>
  struct output_iterator_traits<std::raw_storage_iterator<OutputIt, T>> : std::iterator<std::output_iterator_tag, T> {};

  template <class Container>
  struct output_iterator_traits<std::back_insert_iterator<Container>>
      : std::iterator<std::output_iterator_tag, typename Container::value_type> {};

  template <class Container>
  struct output_iterator_traits<std::front_insert_iterator<Container>>
      : std::iterator<std::output_iterator_tag, typename Container::value_type> {};

  template <class Container>
  struct output_iterator_traits<std::insert_iterator<Container>>
      : std::iterator<std::output_iterator_tag, typename Container::value_type> {};

  template <class T, class charT, class traits>
  struct output_iterator_traits<std::ostream_iterator<T, charT, traits>> : std::iterator<std::output_iterator_tag, T> {
  };

  template <class charT, class traits>
  struct output_iterator_traits<std::ostreambuf_iterator<charT, traits>>
      : std::iterator<std::output_iterator_tag, charT> {};

  template <class T>
  using output_iterator_value_type_t = typename output_iterator_traits<T>::value_type;

} // namespace detail.

namespace detail {
  inline constexpr const std::uint16_t k_lead_surrogate_min = 0xD800u;
  inline constexpr const std::uint16_t k_lead_offset = k_lead_surrogate_min - (0x10000 >> 10);
  inline constexpr const std::uint16_t k_trail_surrogate_min = 0xDC00u;

  /// Maximum valid value for a Unicode code point.
  inline constexpr const std::uint32_t k_code_point_max = 0x0010FFFFu;
  inline constexpr const std::uint32_t k_surrogate_offset
      = 0x10000u - (k_lead_surrogate_min << 10) - k_trail_surrogate_min;

  inline constexpr const std::uint8_t bom[] = { 0xef, 0xbb, 0xbf };

  template <typename u8char_type>
  inline std::uint8_t cast_8(u8char_type c) noexcept {
    return static_cast<std::uint8_t>(c);
  }

  template <typename u16char_type>
  inline std::uint16_t cast_16(u16char_type c) noexcept {
    return static_cast<std::uint16_t>(c);
  }

  template <typename u8char_type>
  inline bool is_trail(u8char_type c) {
    return (cast_8(c) >> 6) == 0x2;
  }

  inline constexpr bool is_surrogate(char16_t uc) noexcept { return (uc - k_lead_surrogate_min) < 2048u; }
  // inline constexpr bool is_surrogate(char16_t uc) noexcept { return (uc - 0xD800u) < 2048u; }

  // Lead.
  inline constexpr bool is_high_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == k_lead_surrogate_min; }
  // inline constexpr bool is_high_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == 0xD800; }

  // Trail.
  inline constexpr bool is_low_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == k_trail_surrogate_min; }
  // inline constexpr bool is_low_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == 0xDC00; }

  template <typename u32char_type>
  inline constexpr bool is_valid_code_point(u32char_type cp) noexcept {
    return cp <= k_code_point_max && !is_surrogate(cp);
  }

  inline constexpr std::size_t sequence_length(std::uint8_t lead) noexcept {
    return lead < 0x80 ? 1 : (lead >> 5) == 0x6 ? 2 : (lead >> 4) == 0xE ? 3 : (lead >> 3) == 0x1E ? 4 : 0;
  }

  inline constexpr std::size_t u16_sequence_length(char16_t c) noexcept {
    std::uint32_t cp = detail::cast_16(c);
    return is_high_surrogate(cp) ? 2 : 1;
  }

  template <typename u8_iterator>
  inline constexpr bool starts_with_bom(u8_iterator it, u8_iterator end) noexcept {
    return (((it != end) && (cast_8(*it++)) == bom[0]) && ((it != end) && (cast_8(*it++)) == bom[1])
        && ((it != end) && (cast_8(*it)) == bom[2]));
  }
} // namespace detail.

template <typename u8_iterator>
inline u8_iterator append_u32_to_u8(std::uint32_t cp, u8_iterator it) {
  // 1 byte.
  if (cp < 0x80) {
    *it++ = detail::cast_8(cp);
  }
  // 2 bytes.
  else if (cp < 0x800) {
    *it++ = detail::cast_8((cp >> 6) | 0xC0);
    *it++ = detail::cast_8((cp & 0x3F) | 0x80);
  }
  // 3 bytes.
  else if (cp < 0x10000) {
    *it++ = detail::cast_8((cp >> 12) | 0xE0);
    *it++ = detail::cast_8(((cp >> 6) & 0x3F) | 0x80);
    *it++ = detail::cast_8((cp & 0x3F) | 0x80);
  }
  // 4 bytes.
  else {
    *it++ = detail::cast_8((cp >> 18) | 0xF0);
    *it++ = detail::cast_8(((cp >> 12) & 0x3F) | 0x80);
    *it++ = detail::cast_8(((cp >> 6) & 0x3F) | 0x80);
    *it++ = detail::cast_8((cp & 0x3F) | 0x80);
  }

  return it;
}

inline std::size_t code_point_size_u8(std::uint32_t cp) {
  // 1 byte.
  if (cp < 0x80) {
    return 1;
  }
  // 2 bytes.
  else if (cp < 0x800) {
    return 2;
  }
  // 3 bytes.
  else if (cp < 0x10000) {
    return 3;
  }
  // 4 bytes.
  else {
    return 4;
  }
}

template <typename u8_iterator>
inline std::uint32_t next_u8_to_u32(u8_iterator& it) {
  std::uint32_t cp = detail::cast_8(*it);

  using difference_type = typename std::iterator_traits<u8_iterator>::difference_type;
  difference_type length = detail::sequence_length(*it);

  switch (length) {
  case 1:
    break;
  case 2:
    it++;
    cp = ((cp << 6) & 0x7FF) + ((*it) & 0x3F);
    break;
  case 3:
    ++it;
    cp = ((cp << 12) & 0xFFFF) + ((detail::cast_8(*it) << 6) & 0xFFF);
    ++it;
    cp += (*it) & 0x3F;
    break;
  case 4:
    ++it;
    cp = ((cp << 18) & 0x1FFFFF) + ((detail::cast_8(*it) << 12) & 0x3FFFF);
    ++it;
    cp += (detail::cast_8(*it) << 6) & 0xFFF;
    ++it;
    cp += (*it) & 0x3F;
    break;
  }
  ++it;
  return cp;
}

template <typename octet_iterator>
std::uint32_t prior_u8_to_u32(octet_iterator& it) {
  while (detail::is_trail(*(--it)))
    ;

  octet_iterator temp = it;
  return next_u8_to_u32(temp);
}

template <typename u16_iterator, typename u8_iterator>
u16_iterator u8_to_u16(u8_iterator start, u8_iterator end, u16_iterator outputIt) {
  while (start < end) {
    std::uint32_t cp = next_u8_to_u32(start);

    if (cp > 0xFFFF) { // make a surrogate pair
      *outputIt++ = detail::cast_16((cp >> 10) + detail::k_lead_offset);
      *outputIt++ = detail::cast_16((cp & 0x3FF) + detail::k_trail_surrogate_min);
    }
    else {
      *outputIt++ = detail::cast_16(cp);
    }
  }

  return outputIt;
}

template <typename u8_iterator>
std::size_t u8_to_u16_length(u8_iterator start, u8_iterator end) {
  std::size_t count = 0;
  while (start < end) {
    std::uint32_t cp = next_u8_to_u32(start);
    count += (cp > 0xFFFF) ? 2 : 1;
  }

  return count;
}

template <typename u32_iterator, typename u8_iterator>
u32_iterator u8_to_u32(u8_iterator start, u8_iterator end, u32_iterator outputIt) {
  while (start < end) {
    *outputIt++ = next_u8_to_u32(start);
  }

  return outputIt;
}

template <typename u8_iterator>
std::size_t u8_to_u32_length(u8_iterator start, u8_iterator end) {
  std::size_t count = 0;
  while (start < end) {
    switch (detail::sequence_length(*start++)) {
    case 1:
      break;
    case 2:
      ++start;
      break;
    case 3:
      ++start;
      ++start;
      break;
    case 4:
      ++start;
      ++start;
      ++start;
      break;
    }
    count++;
  }

  return count;
}

template <typename CharT, std::enable_if_t<sizeof(CharT) == sizeof(char), std::nullptr_t> = nullptr>
std::size_t u8_length(const CharT* str, std::size_t size) noexcept {
  std::size_t dist = 0;

  for (std::size_t i = 0; i < size; i += detail::sequence_length(str[i])) {
    dist++;
  }

  return dist;
}

template <typename u16_iterator, typename u8_iterator>
u8_iterator u16_to_u8(u16_iterator start, u16_iterator end, u8_iterator outputIt) {
  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    outputIt = append_u32_to_u8(cp, outputIt);
  }

  return outputIt;
}

template <typename u16_iterator, typename u32_iterator>
u32_iterator u16_to_u32(u16_iterator start, u16_iterator end, u32_iterator outputIt) {
  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    *outputIt++ = cp;
  }

  return outputIt;
}

template <typename u16_iterator>
std::size_t u16_to_u8_length(u16_iterator start, u16_iterator end) {
  std::size_t count = 0;
  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    count += code_point_size_u8(cp);
  }

  return count;
}

template <typename u16_iterator>
std::size_t u16_to_u32_length(u16_iterator start, u16_iterator end) {
  std::size_t count = 0;

  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    count++;
  }

  return count;
}

template <typename CharT, std::enable_if_t<sizeof(CharT) == sizeof(char16_t), std::nullptr_t> = nullptr>
std::size_t u16_length(const CharT* str, std::size_t size) noexcept {
  std::size_t dist = 0;

  for (std::size_t i = 0; i < size; i++) {
    if (detail::is_high_surrogate(detail::cast_16(str[i]))) {
      i++;
    }

    dist++;
  }

  return dist;
}

template <typename u8_iterator, typename u32_iterator>
u8_iterator u32_to_u8(u32_iterator start, u32_iterator end, u8_iterator outputIt) {
  while (start != end) {
    outputIt = append_u32_to_u8(*start++, outputIt);
  }

  return outputIt;
}

template <typename u16_iterator, typename u32_iterator>
u16_iterator u32_to_u16(u32_iterator start, u32_iterator end, u16_iterator outputIt) {
  while (start != end) {
    std::uint32_t cp = static_cast<std::uint32_t>(*start++);

    using value_type = utf::detail::output_iterator_value_type_t<u16_iterator>;

    if (cp <= 0x0000FFFF) {
      // UTF-16 surrogate values are illegal in UTF-32
      // 0xFFFF or 0xFFFE are both reserved values.
      if (cp >= 0xD800 && cp <= 0xDFFF) {
        *outputIt++ = 0x0000FFFD;
      }
      else {
        // BMP character.
        *outputIt++ = static_cast<value_type>(cp);
      }
    }
    else if (cp > 0x0010FFFF) {
      // U+10FFFF is the largest code point of Unicode character set.
      *outputIt++ = static_cast<value_type>(0x0000FFFD);
    }
    else {
      // c32 is a character in range 0xFFFF - 0x10FFFF.
      cp -= 0x0010000UL;
      *outputIt++ = static_cast<value_type>(((cp >> 10) + 0xD800));
      *outputIt++ = static_cast<value_type>(((cp & 0x3FFUL) + 0xDC00));
    }
  }

  return outputIt;
}

template <typename u32_iterator>
std::size_t u32_to_u8_length(u32_iterator start, u32_iterator end) {
  std::size_t count = 0;

  while (start != end) {
    count += static_cast<std::size_t>(code_point_size_u8(*start++));
  }

  return count;
}

template <typename u32_iterator>
std::size_t u32_to_u16_length(u32_iterator start, u32_iterator end) {
  std::size_t count = 0;

  while (start != end) {
    std::uint32_t cp = static_cast<std::uint32_t>(*start++);
    count += (cp <= 0x0000FFFF || cp > 0x0010FFFF) ? 1 : 2;
  }

  return count;
}

template <typename CharT, std::enable_if_t<sizeof(CharT) == sizeof(char32_t), std::nullptr_t> = nullptr>
std::size_t u32_length(const CharT*, std::size_t size) noexcept {
  return size;
}

template <typename CharT, typename SType,
    std::enable_if_t<is_string_type<SType>::value && is_char_type<CharT>::value, std::nullptr_t>>
inline std::size_t convert_size(const SType& str) {
  using input_char_type = utf::string_char_type_t<SType>;
  constexpr encoding input_encoding = utf::encoding_of<input_char_type>::value;

  using output_char_type = CharT;
  constexpr encoding output_encoding = utf::encoding_of<output_char_type>::value;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    if constexpr (output_encoding == encoding::utf8) {
      return input_view.size();
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u8_to_u16_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u8_to_u32_length(input_view.begin(), input_view.end());
    }
    else {
      return 0;
    }
  }
  else if constexpr (input_encoding == encoding::utf16) {
    if constexpr (output_encoding == encoding::utf8) {
      return u16_to_u8_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return input_view.size();
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u16_to_u32_length(input_view.begin(), input_view.end());
    }
    else {
      return 0;
    }
  }
  else if constexpr (input_encoding == encoding::utf32) {
    if constexpr (output_encoding == encoding::utf8) {
      return u32_to_u8_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u32_to_u16_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return input_view.size();
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }
}

template <typename CharT, typename SType,
    std::enable_if_t<is_string_type<SType>::value && is_char_type<CharT>::value, std::nullptr_t>>
inline std::basic_string<CharT> convert_as(const SType& str) {
  using input_char_type = utf::string_char_type_t<SType>;
  constexpr encoding input_encoding = utf::encoding_of<input_char_type>::value;

  using output_char_type = CharT;
  constexpr encoding output_encoding = utf::encoding_of<output_char_type>::value;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    if constexpr (output_encoding == encoding::utf8) {
      return std::basic_string<output_char_type>((const output_char_type*)input_view.data(), input_view.size());
    }
    else if constexpr (output_encoding == encoding::utf16) {
      std::basic_string<output_char_type> output;
      u8_to_u16(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf32) {
      std::basic_string<output_char_type> output;
      u8_to_u32(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else {
      return {};
    }
  }
  else if constexpr (input_encoding == encoding::utf16) {
    if constexpr (output_encoding == encoding::utf8) {
      std::basic_string<output_char_type> output;
      u16_to_u8(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return std::basic_string<output_char_type>((const output_char_type*)input_view.data(), input_view.size());
    }
    else if constexpr (output_encoding == encoding::utf32) {
      std::basic_string<output_char_type> output;
      u16_to_u32(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else {
      return {};
    }
  }
  else if constexpr (input_encoding == encoding::utf32) {
    if constexpr (output_encoding == encoding::utf8) {
      std::basic_string<output_char_type> output;
      u32_to_u8(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf16) {
      std::basic_string<output_char_type> output;
      u32_to_u16(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return std::basic_string<output_char_type>((const output_char_type*)input_view.data(), input_view.size());
    }
    else {
      return {};
    }
  }
  else {
    return {};
  }
}

template <class SType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t>>
class convert {
public:
  using input_char_type = utf::string_char_type_t<SType>;
  static constexpr encoding input_encoding = utf::encoding_of<input_char_type>::value;

  inline convert(const SType& str)
      : _input_view(str) {}

  template <typename CharT>
  inline operator std::basic_string<CharT>() const {
    return convert_as<CharT>(_input_view);
  }

private:
  std::basic_string_view<input_char_type> _input_view;
};

template <class SType, class OutputIt, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t>>
inline OutputIt copy(const SType& str, OutputIt outputIt) {

  using input_char_type = utf::string_char_type_t<SType>;
  constexpr encoding input_encoding = utf::encoding_of<input_char_type>::value;

  using output_char_type = detail::output_iterator_value_type_t<OutputIt>;
  constexpr encoding output_encoding = utf::encoding_of<output_char_type>::value;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    if constexpr (output_encoding == encoding::utf8) {
      for (std::size_t i = 0; i < input_view.size(); i++) {
        *outputIt++ = (output_char_type)input_view[i];
      }
      return outputIt;
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u8_to_u16(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u8_to_u32(input_view.begin(), input_view.end(), outputIt);
    }
  }
  else if constexpr (input_encoding == encoding::utf16) {
    if constexpr (output_encoding == encoding::utf8) {
      return u16_to_u8(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf16) {
      for (std::size_t i = 0; i < input_view.size(); i++) {
        *outputIt++ = (output_char_type)input_view[i];
      }
      return outputIt;
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u16_to_u32(input_view.begin(), input_view.end(), outputIt);
    }
  }
  else if constexpr (input_encoding == encoding::utf32) {
    if constexpr (output_encoding == encoding::utf8) {
      return u32_to_u8(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u32_to_u16(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf32) {
      for (std::size_t i = 0; i < input_view.size(); i++) {
        *outputIt++ = (output_char_type)input_view[i];
      }
      return outputIt;
    }
  }
}

template <class SType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t>>
inline std::size_t length(const SType& str) {
  using input_char_type = utf::string_char_type_t<SType>;
  constexpr encoding input_encoding = utf::encoding_of<input_char_type>::value;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    return u8_length(input_view.data(), input_view.size());
  }
  else if constexpr (input_encoding == encoding::utf16) {
    return u16_length(input_view.data(), input_view.size());
  }
  else if constexpr (input_encoding == encoding::utf32) {
    return u32_length(input_view.data(), input_view.size());
  }
  else {
    return 0;
  }
}

namespace detail {
  template <encoding Encoding = encoding::utf8>
  struct iterator_sequence_length {
    static inline constexpr std::size_t length(std::uint8_t lead) noexcept { return sequence_length(lead); }
  };

  template <>
  struct iterator_sequence_length<encoding::utf16> {
    static inline constexpr std::size_t length(std::int16_t c) noexcept { return u16_sequence_length((char16_t)c); }
  };

  template <>
  struct iterator_sequence_length<encoding::utf32> {
    static inline constexpr std::size_t length(std::int32_t) noexcept { return 1; }
  };

  template <typename InputCharT, typename OutputChart, typename = void>
  struct base_iterator {};

  template <typename InputCharT, typename OutputCharT>
  struct base_iterator<InputCharT, OutputCharT,
      std::enable_if_t<encoding_of<InputCharT>::value == encoding_of<OutputCharT>::value>> {

    using input_char_type = InputCharT;
    using output_char_type = OutputCharT;

    using input_view_type = std::basic_string_view<input_char_type>;
    using output_view_type = std::basic_string_view<output_char_type>;

    using it_seq_length = detail::iterator_sequence_length<encoding_of<InputCharT>::value>;

    template <typename Iterator>
    static inline output_view_type get(Iterator it) {
      return output_view_type((const output_char_type*)&(it[0]), it_seq_length::length(*it));
    }

    template <typename Iterator>
    static inline void advance(Iterator& it) {
      std::advance(it, it_seq_length::length(*it));
    }
  };

  template <typename InputCharT, typename OutputCharT>
  struct base_iterator<InputCharT, OutputCharT,
      std::enable_if_t<encoding_of<InputCharT>::value != encoding_of<OutputCharT>::value>> {

    using input_char_type = InputCharT;
    using output_char_type = OutputCharT;

    using input_view_type = std::basic_string_view<input_char_type>;
    using output_view_type = std::basic_string_view<output_char_type>;

    using it_seq_length = detail::iterator_sequence_length<encoding_of<InputCharT>::value>;

    template <typename Iterator>
    inline output_view_type get(Iterator it) const {
      return output_view_type(_data.begin(),
          std::distance(_data.begin(), utf::copy(input_view_type(&it[0], it_seq_length::length(*it)), _data.begin())));
    }

    template <typename Iterator>
    inline void advance(Iterator& it) {
      std::advance(it, it_seq_length::length(*it));
    }

    mutable std::array<output_char_type, encoding_to_max_char_count<encoding_of<OutputCharT>::value>::value> _data;
  };

  template <class IteratorType>
  using iterator_value_type = utf::remove_cvref_t<decltype(std::declval<IteratorType>()[0])>;

  template <class IteratorType>
  using iterator_base_type = basic_iterator<iterator_value_type<IteratorType>,
      std::basic_string_view<iterator_value_type<IteratorType>>, IteratorType>;

  template <typename IteratorT>
  class iterator_range {
    IteratorT _begin_iterator;
    IteratorT _end_iterator;

  public:
    template <typename Container>
    inline iterator_range(Container&& c)
        : _begin_iterator(c.begin())
        , _end_iterator(c.end()) {}

    inline iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
        : _begin_iterator(std::move(begin_iterator))
        , _end_iterator(std::move(end_iterator)) {}

    inline IteratorT begin() const { return _begin_iterator; }
    inline IteratorT end() const { return _end_iterator; }
    inline bool empty() const { return _begin_iterator == _end_iterator; }
  };

} // namespace detail.

template <class CharT, class SType, class IteratorType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t>>
class basic_iterator : private detail::base_iterator<utf::string_char_type_t<SType>, CharT> {
  using base_type = detail::base_iterator<utf::string_char_type_t<SType>, CharT>;
  using output_view_type = typename base_type::output_view_type;

  template <class T>
  friend class iterator;

public:
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;

  inline basic_iterator() = default;

  inline explicit basic_iterator(IteratorType it)
      : _it(it) {}

  inline IteratorType base() const { return _it; }

  inline output_view_type operator*() const { return base_type::get(_it); }

  inline bool operator==(const basic_iterator& rhs) const { return (_it == rhs._it); }

  inline bool operator!=(const basic_iterator& rhs) const { return !(operator==(rhs)); }

  inline basic_iterator& operator++() {
    base_type::advance(_it);
    return *this;
  }

  inline basic_iterator operator++(int) {
    basic_iterator temp = *this;
    base_type::advance(_it);
    return temp;
  }

private:
  IteratorType _it;
};

template <typename CharT, typename SType>
inline auto iterate_as(const SType& str) {
  return detail::iterator_range<basic_iterator<CharT, SType, typename SType::const_iterator>>(str);
}

template <typename SType, std::enable_if_t<is_string_type<SType>::value, std::nullptr_t>>
inline auto iterate(const SType& str) {
  return detail::iterator_range<basic_iterator<string_char_type_t<SType>, SType, typename SType::const_iterator>>(str);
}

// template <typename SType,
//     std::enable_if_t<std::is_constructible<SType, std::string_view>::value, std::nullptr_t> = nullptr>
// inline auto iterate(const SType& str) {
//   return detail::iterator_range<basic_iterator<char, SType, typename SType::const_iterator>>(str);
// }
//
//#ifdef UNICODE_CPP_20
// template <typename SType,
//     std::enable_if_t<std::is_constructible<SType, std::u8string_view>::value, std::nullptr_t> = nullptr>
// inline auto iterate(const SType& str) {
//   return detail::iterator_range<basic_iterator<char8_t, SType, typename SType::const_iterator>>(str);
// }
//#endif // UNICODE_CPP_20
//
// template <typename SType,
//     std::enable_if_t<std::is_constructible<SType, std::u16string_view>::value, std::nullptr_t> = nullptr>
// inline auto iterate(const SType& str) {
//   return detail::iterator_range<basic_iterator<char16_t, SType, typename SType::const_iterator>>(str);
// }
//
// template <typename SType,
//     std::enable_if_t<std::is_constructible<SType, std::u32string_view>::value, std::nullptr_t> = nullptr>
// inline auto iterate(const SType& str) {
//   return detail::iterator_range<basic_iterator<char32_t, SType, typename SType::const_iterator>>(str);
// }
//
// template <typename SType,
//     std::enable_if_t<std::is_constructible<SType, std::wstring_view>::value, std::nullptr_t> = nullptr>
// inline auto iterate(const SType& str) {
//   return detail::iterator_range<basic_iterator<wchar_t, SType, typename SType::const_iterator>>(str);
// }

template <class IteratorType>
class iterator : private detail::iterator_base_type<IteratorType> {
  using base_type = detail::iterator_base_type<IteratorType>;

public:
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;

  inline iterator() = default;

  inline explicit iterator(IteratorType it)
      : _it(it) {}

  inline IteratorType base() const { return _it; }

  inline typename base_type::output_view_type operator*() const { return base_type::get(_it); }

  inline bool operator==(const iterator& rhs) const { return (_it == rhs._it); }

  inline bool operator!=(const iterator& rhs) const { return !(operator==(rhs)); }

  inline iterator& operator++() {
    base_type::advance(_it);
    return *this;
  }

  inline iterator operator++(int) {
    iterator temp = *this;
    base_type::advance(_it);
    return temp;
  }

private:
  IteratorType _it;
};

//
//
//
template <typename T, std::enable_if_t<is_string_type<remove_cvref_t<T>>::value, std::nullptr_t>>
string_view::string_view(T&& s)
    : string_view(make<T>(std::forward<T>(s))) {}

string_view::string_view() noexcept
    : m_data()
    , m_size(0)
    , m_charSize(sizeof(char))
    , m_nullTerminated(false) {}

string_view::string_view(const std::string& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char))
    , m_nullTerminated(true) {}

string_view::string_view(const std::u16string& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char16_t))
    , m_nullTerminated(true) {}

string_view::string_view(const std::u32string& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char32_t))
    , m_nullTerminated(true) {}

string_view::string_view(const std::wstring& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(wchar_t))
    , m_nullTerminated(true) {}

string_view::string_view(std::string_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char))
    , m_nullTerminated(false) {}

string_view::string_view(std::u16string_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char16_t))
    , m_nullTerminated(false) {}

string_view::string_view(std::u32string_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char32_t))
    , m_nullTerminated(false) {}

string_view::string_view(std::wstring_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(wchar_t))
    , m_nullTerminated(false) {}

string_view::string_view(std::string_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char))
    , m_nullTerminated(true) {}

string_view::string_view(std::u16string_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char16_t))
    , m_nullTerminated(true) {}

string_view::string_view(std::u32string_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char32_t))
    , m_nullTerminated(true) {}

string_view::string_view(std::wstring_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(wchar_t))
    , m_nullTerminated(true) {}

string_view::content::content() noexcept
    : c8(nullptr) {}

string_view::content::content(const char* s) noexcept
    : c8(s) {}

string_view::content::content(const char16_t* s) noexcept
    : c16(s) {}

string_view::content::content(const char32_t* s) noexcept
    : c32(s) {}

string_view::content::content(const wchar_t* s) noexcept
    : content([](const wchar_t* s) {
      if constexpr (sizeof(wchar_t) == sizeof(char16_t)) {
        return content((const char16_t*)s);
      }
      else {
        return content((const char32_t*)s);
      }
    }(s)) {}

const wchar_t* string_view::content::cw() const noexcept {
  if constexpr (sizeof(wchar_t) == sizeof(char16_t)) {
    return (const wchar_t*)(const void*)c16;
  }
  else {
    return (const wchar_t*)c32;
  }
}

encoding string_view::encoding() const noexcept {
  switch (char_size()) {
  case sizeof(char):
    return encoding::utf8;
  case sizeof(char16_t):
    return encoding::utf16;
  case sizeof(char32_t):
    return encoding::utf32;
  default:
    return encoding::utf8;
  }
}

template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t>>
const CharT* string_view::c_str() const noexcept {
  if constexpr (std::is_same_v<CharT, char>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(char) && m_nullTerminated) ? m_data.c8 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(char16_t) && m_nullTerminated) ? m_data.c16 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(char32_t) && m_nullTerminated) ? m_data.c32 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(wchar_t) && m_nullTerminated) ? m_data.cw() : nullptr;
  }

  else {
    // Should never happen.
    //    mts_error("Wrong char type");
    return nullptr;
  }
}

template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t>>
const CharT* string_view::data() const noexcept {
  if constexpr (std::is_same_v<CharT, char>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char), "Wrong char type");
    return (m_charSize == sizeof(char)) ? m_data.c8 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t), "Wrong char type");
    return (m_charSize == sizeof(char16_t)) ? m_data.c16 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t), "Wrong char type");
    return (m_charSize == sizeof(char32_t)) ? m_data.c32 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t), "Wrong char type");
    return (m_charSize == sizeof(wchar_t)) ? m_data.cw() : nullptr;
  }

  else {
    // Should never happen.
    //    mts_error("Wrong char type");
    return nullptr;
  }
}

template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t>>
std::basic_string<CharT> string_view::str() const {
  using stype = std::basic_string<CharT>;

  if constexpr (std::is_same_v<CharT, char>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char), "Wrong char type");
    return (m_charSize == sizeof(char)) ? stype(m_data.c8, m_size) : stype();
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t), "Wrong char type");
    return (m_charSize == sizeof(char16_t)) ? stype(m_data.c16, m_size) : stype();
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t), "Wrong char type");
    return (m_charSize == sizeof(char32_t)) ? stype(m_data.c32, m_size) : stype();
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t), "Wrong char type");
    return (m_charSize == sizeof(wchar_t)) ? stype(m_data.cw(), m_size) : stype();
  }

  else {
    // Should never happen.
    //    mts_error("Wrong char type");
    return stype();
  }
}

template <typename CharT, std::enable_if_t<is_char_type<CharT>::value, std::nullptr_t>>
std::basic_string_view<CharT> string_view::view() const {
  using svtype = std::basic_string_view<CharT>;

  if constexpr (std::is_same_v<CharT, char>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char), "Wrong char type");
    return (m_charSize == sizeof(char)) ? svtype(m_data.c8, m_size) : svtype();
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t), "Wrong char type");
    return (m_charSize == sizeof(char16_t)) ? svtype(m_data.c16, m_size) : svtype();
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t), "Wrong char type");
    return (m_charSize == sizeof(char32_t)) ? svtype(m_data.c32, m_size) : svtype();
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    //    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t), "Wrong char type");
    return (m_charSize == sizeof(wchar_t)) ? svtype(m_data.cw(), m_size) : svtype();
  }

  else {
    // Should never happen.
    //    mts_error("Wrong char type");
    return svtype();
  }
}

template <typename T>
auto string_view::make(T&& s) {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;

  constexpr bool is_any_string_or_string_view = std::is_same_v<type, std::string> //
      || std::is_same_v<type, std::u16string> //
      || std::is_same_v<type, std::u32string> //
      || std::is_same_v<type, std::wstring> //
      || std::is_same_v<type, std::string_view> //
      || std::is_same_v<type, std::u16string_view> //
      || std::is_same_v<type, std::u32string_view> //
      || std::is_same_v<type, std::wstring_view>;

  static_assert(is_any_string_or_string_view || is_string_type<T>::value, "");

  if constexpr (is_any_string_or_string_view) {
    return string_view(std::forward<T>(s), normal_tag{});
  }

  else if constexpr (std::is_constructible_v<std::string_view, T>) {
    return string_view(std::string_view(s), null_terminated_tag{});
  }

  else if constexpr (std::is_constructible_v<std::u16string_view, T>) {
    return string_view(std::u16string_view(s), null_terminated_tag{});
  }

  else if constexpr (std::is_constructible_v<std::u32string_view, T>) {
    return string_view(std::u32string_view(s), null_terminated_tag{});
  }

  else if constexpr (std::is_constructible_v<std::wstring_view, T>) {
    return string_view(std::wstring_view(s), null_terminated_tag{});
  }

  else {
    // Should never happen.
    // mts_error("Wrong char type");
    return string_view();
  }
}

std::string to_utf8(string_view s) { return s.to_utf8(); }

std::u16string to_utf16(string_view s) { return s.to_utf16(); }

std::u32string to_utf32(string_view s) { return s.to_utf32(); }

std::wstring to_wide(string_view s) { return s.to_wide(); }

std::string string_view::to_utf8() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case encoding::utf8:
    return std::string(data<char>(), size());

  case encoding::utf16:
    return convert_as<char>(view<char16_t>());

  case encoding::utf32:
    return convert_as<char>(view<char32_t>());
  }

  return {};
}

std::u16string string_view::to_utf16() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case encoding::utf8:
    return convert_as<char16_t>(view<char>());

  case encoding::utf16:

    return std::u16string(data<char16_t>(), size());

  case encoding::utf32:
    return convert_as<char16_t>(view<char32_t>());
  }

  return {};
}

std::u32string string_view::to_utf32() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case encoding::utf8:
    return convert_as<char32_t>(view<char>());

  case encoding::utf16:
    return convert_as<char32_t>(view<char16_t>());

  case encoding::utf32:
    return std::u32string(data<char32_t>(), size());
  }

  return {};
}

std::wstring string_view::to_wide() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case encoding::utf8:
    return convert_as<wchar_t>(view<char>());

  case encoding::utf16:
    return convert_as<wchar_t>(view<char16_t>());

  case encoding::utf32:
    return convert_as<wchar_t>(view<char32_t>());
  }

  return {};
}

std::size_t string_view::count() const {
  switch (encoding()) {
  case encoding::utf8:
    return utf::length(view<char>());

  case encoding::utf16:
    return utf::length(view<char16_t>());

  case encoding::utf32:
    return size();
  }
  return 0;
}

inline const char* string_view::u8cstr() const noexcept { return c_str<char>(); }

/// Same as c_str<char16_t>().
inline const char16_t* string_view::u16cstr() const noexcept { return c_str<char16_t>(); }

/// Same as c_str<char32_t>().
inline const char32_t* string_view::u32cstr() const noexcept { return c_str<char32_t>(); }

/// Same as c_str<wchar_t>().
inline const wchar_t* string_view::wcstr() const noexcept { return c_str<wchar_t>(); }

/// Same as data<char>().
inline const char* string_view::u8data() const noexcept { return data<char>(); }

/// Same as data<char16_t>().
inline const char16_t* string_view::u16data() const noexcept { return data<char16_t>(); }

/// Same as data<char32_t>().
inline const char32_t* string_view::u32data() const noexcept { return data<char32_t>(); }

/// Same as data<wchar_t>().
inline const wchar_t* string_view::wdata() const noexcept { return data<wchar_t>(); }

/// Same as str<char>()
inline std::string string_view::u8str() const noexcept { return str<char>(); }

/// Same as str<char16_t>()
inline std::basic_string<char16_t> string_view::u16str() const noexcept { return str<char16_t>(); }

/// Same as str<char32_t>()
inline std::basic_string<char32_t> string_view::u32str() const noexcept { return str<char32_t>(); }

/// Same as str<wchar_t>()
inline std::wstring string_view::wstr() const noexcept { return str<wchar_t>(); }

/// Same as view<char>().
inline std::string_view string_view::u8view() const noexcept { return view<char>(); }

/// Same as view<char16_t>().
inline std::basic_string_view<char16_t> string_view::u16view() const noexcept { return view<char16_t>(); }

/// Same as view<char32_t>().
inline std::basic_string_view<char32_t> string_view::u32view() const noexcept { return view<char32_t>(); }

/// Same as view<wchar_t>().
inline std::wstring_view string_view::wview() const noexcept { return view<wchar_t>(); }
} // namespace utf
