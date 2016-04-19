/*
  GWEN
  Copyright (c) 2010 Facepunch Studios
  See license in Gwen.h
*/

#pragma once
#ifndef GWEN_UTILITY_H
#define GWEN_UTILITY_H

#include <sstream>
#include <vector>
#include <Gwen/Structures.h>

namespace Gwen {
class TextObject;

namespace Utility {
template <typename T> const T &Max(const T &x, const T &y) {
  if (y < x) {
    return x;
  }

  return y;
}

template <typename T> const T &Min(const T &x, const T &y) {
  if (y > x) {
    return x;
  }

  return y;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

inline String UnicodeToString(const UnicodeString &strIn) {
  if (!strIn.length()) {
    return "";
  }

  String strOut;
  for (String::size_type i = 0; i < strIn.size(); ++i) {
    String::value_type chr = strIn[i];
    if (chr >= 0 && chr <= 0x007F) {
      strOut.push_back(chr);
    } else if (chr >= 0x0080 && chr <= 0x07FF) {
      strOut.push_back(0xC0 | ((chr >> 6) & 0x1F));
      strOut.push_back(0x80 | (chr & 0x3F));
    } else if (chr >= 0x0800 && chr <= 0xFFFF) {
      strOut.push_back(0xE0 | ((chr >> 12) & 0x0F));
      strOut.push_back(0x80 | ((chr >> 6) & 0x3F));
      strOut.push_back(0x80 | (chr & 0x3F));
    } else if (chr >= 0x10000 && chr <= 0x10FFFF) {
      strOut.push_back(0xF0 | ((chr >> 16) & 0x07));
      strOut.push_back(0x80 | ((chr >> 12) & 0x3F));
      strOut.push_back(0x80 | ((chr >> 6) & 0x3F));
      strOut.push_back(0x80 | (chr & 0x3F));
    } else {
      strOut.append("\xEF\xBF\xBD");
    }
  }

  return strOut;
}

inline UnicodeString StringToUnicode(const String &strIn) {
  if (!strIn.length()) {
    return U"";
  }

  UnicodeString strOut;
  char32_t uchr;
  int multibyte = 0;
  for (UnicodeString::size_type i = 0; i < strIn.size(); ++i) {
    UnicodeString::value_type c = strIn[i];
    if (multibyte == 0) {
      if (c <= 0x7F) {
        strOut.push_back(c);
      } else if ((c & 0xF8) == 0xF8) {
        // 5+ byte character, prohibited
        // Skip continuation bytes
        while ((((c = strIn[++i]) & 0xC0) == 0x80) && i < strIn.size())
          ;
        strOut.push_back(0xFFFD);
      } else if ((c & 0b11111000) == 0b11110000) {
        multibyte = 3;
        uchr = c & 0x07;
      } else if ((c & 0b11110000) == 0b11100000) {
        multibyte = 2;
        uchr = c & 0x0F;
      } else if ((c & 0b11100000) == 0b11000000) {
        multibyte = 1;
        uchr = c & 0x1F;
      } else {
        strOut.push_back(0xFFFD);
      }
    } else {
      if ((c & 0xC0) == 0x80) {
        uchr <<= 6;
        uchr |= c & 0x3F;
      } else {
        // Unexpected end of multibyte codepoint
        strOut.push_back(0xFFFD);
        // Roll back 1 char (re-loop on current)
        --i;
        multibyte = 0;
        continue;
      }
      if (--multibyte == 0) {
        if (uchr <= 0x10FFFF) {
          strOut.push_back(uchr);
        } else {
          strOut.push_back(0xFFFD);
        }
      }
    }
  }

  return strOut;
}

template <typename T> void Replace(T &str, const T &strFind, const T &strReplace) {
  size_t pos = 0;

  while ((pos = str.find(strFind, pos)) != T::npos) {
    str.replace(pos, strFind.length(), strReplace);
    pos += strReplace.length();
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

template <class T> String ToString(const T &object) {
  std::ostringstream os;
  os << object;
  return os.str();
}

inline Gwen::Rect ClampRectToRect(Gwen::Rect inside, Gwen::Rect outside, bool clampSize = false) {
  if (inside.x < outside.x) {
    inside.x = outside.x;
  }

  if (inside.y < outside.y) {
    inside.y = outside.y;
  }

  if (inside.x + inside.w > outside.x + outside.w) {
    if (clampSize) {
      inside.w = outside.w;
    } else {
      inside.x = outside.x + outside.w - inside.w;
    }
  }

  if (inside.y + inside.h > outside.y + outside.h) {
    if (clampSize) {
      inside.h = outside.h;
    } else {
      inside.y = outside.w + outside.h - inside.h;
    }
  }

  return inside;
}

GWEN_EXPORT UnicodeString Format(const UnicodeChar *fmt, ...);

namespace Strings {
typedef std::vector<Gwen::String> List;
typedef std::vector<Gwen::UnicodeString> UnicodeList;

GWEN_EXPORT void Split(const Gwen::String &str, const Gwen::String &seperator,
                       Strings::List &outbits, bool bLeaveSeperators = false);
GWEN_EXPORT void Split(const Gwen::UnicodeString &str, const Gwen::UnicodeString &seperator,
                       Strings::UnicodeList &outbits, bool bLeaveSeperators = false);
GWEN_EXPORT bool Wildcard(const Gwen::TextObject &strWildcard, const Gwen::TextObject &strHaystack);

GWEN_EXPORT void ToUpper(Gwen::UnicodeString &str);
GWEN_EXPORT void Strip(Gwen::UnicodeString &str, const Gwen::UnicodeString &chars);

template <typename T> T TrimLeft(const T &str, const T &strChars) {
  T outstr = str;
  outstr.erase(0, outstr.find_first_not_of(strChars));
  return outstr;
}

namespace To {
GWEN_EXPORT bool Bool(const Gwen::String &str);
GWEN_EXPORT int Int(const Gwen::String &str);
GWEN_EXPORT float Float(const Gwen::String &str);
GWEN_EXPORT float Float(const Gwen::UnicodeString &str);
GWEN_EXPORT bool Floats(const Gwen::String &str, float *f, size_t iCount);
}
}
}
}
#endif
