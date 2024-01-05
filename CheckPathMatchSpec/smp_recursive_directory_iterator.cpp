#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <filesystem>

bool is_sep(uint32_t c) { return c == '/' || c == '\\'; }

uint32_t to_lower(uint32_t c) { return (c < 0xffff) ? uint32_t(CharLowerW((LPWSTR)c)) : c; }

uint32_t get_ch(wchar_t const*& s) {
    uint32_t c = *s;
    if (c) {
        ++s;
        if (c >= 0xD800 && c <= 0xDBFF && *s)
            c = (c << 16) | uint32_t(*s++);
    }
    return c;
}

bool fn_match(wchar_t const* ptn, wchar_t const* tgt) {
    wchar_t const* tgt2 = tgt;
    uint32_t tc = get_ch(tgt2);
    switch (*ptn) {
    case L'\0': return tc == L'\0';
    case L'\\':
    case L'/': return is_sep(tc) && fn_match(ptn + 1, tgt2);
    case L'?': return tc && !is_sep(tc) && fn_match(ptn + 1, tgt2);
    case L'*': return fn_match(ptn + 1, tgt) || (tc && !is_sep(tc) && fn_match(ptn, tgt2));
    default:
        uint32_t pc = get_ch(ptn);
        pc = to_lower(pc);
        tc = to_lower(tc);
        return (pc == tc) && fn_match(ptn, tgt2);
    }
}

namespace  fs = std::filesystem;

int wmain(int argc, wchar_t* argv[]) {
	auto dir = (argc >= 2) ? argv[1] : L".";
    for (auto& it : fs::recursive_directory_iterator(dir)) {
        if (fn_match(L"*.cpp", it.path().filename().c_str())) {
            printf("%s\n", (char const*)it.path().u8string().c_str());
        }
    }
	return 0;
}
