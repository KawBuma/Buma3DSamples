#include "pch.h"
#include "Utils.h"

namespace buma
{
namespace util
{

std::string ConvertWideToCp(CODEPAGE _code_page, int _len_with_null_term, const wchar_t* _wstr)
{
    auto l = WideCharToMultiByte(_code_page, 0, _wstr, _len_with_null_term, nullptr, 0, nullptr, FALSE);
    if (l == 0) return std::string();

    std::string str(l - 1, '\0');// 結果のUnicode文字列にはnull終端文字があり、関数によって返される長さにはこの文字が含まれます。
    if (WideCharToMultiByte(_code_page, 0, _wstr, _len_with_null_term, str.data(), l, nullptr, FALSE) == 0)
        return std::string();

    return str;
}

std::wstring ConvertCpToWide(CODEPAGE _code_page, int _len_with_null_term, const char* _str)
{
    auto l = MultiByteToWideChar(_code_page, 0, _str, _len_with_null_term, nullptr, 0);
    if (l == 0)
        return std::wstring();

    std::wstring str(l - 1, L'\0');// 結果のUnicode文字列にはnull終端文字があり、関数によって返される長さにはこの文字が含まれます。
    if (MultiByteToWideChar(_code_page, 0, _str, _len_with_null_term, str.data(), l) == 0)
        return std::wstring();

    return str;
}


}// namespace util
}// namespace buma
