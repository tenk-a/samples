// PathMatchSpecExW の代用処理でのチェック.
// CheckPathMatchSpec.exe 実行後のworkファイル生成済の状態で行うこと.
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


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


int wmain() {
    int sav_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(65001);

    // ASCII以外で表示可能そうな文字を抽出.
    static wchar_t wbuf[0x10000] = {0};
    wchar_t* w = wbuf;
    for (unsigned i = 0x80; i < 0xfffe; ++i) {
        if (i < 0xD800 || i >= 0xE000)
            *w++ = wchar_t(i);
    }
    *w++ = 0;
    static char bbuf[0x10000 * 4] = {0};
    WideCharToMultiByte(65001, 0, wbuf, w - wbuf, bbuf, sizeof bbuf, nullptr, nullptr);
    MultiByteToWideChar(65001, 0, bbuf, strlen(bbuf)+1, wbuf, sizeof wbuf);
    unsigned n_wc = unsigned(wcslen(wbuf));

    // 作業ディレクトリ.
    wchar_t const* wkdir = L"work";
    SetCurrentDirectoryW(wkdir);

    wchar_t  wfname[16] = {0};
    char     bfname[64] = {0};

    // 抽出した文字ごとのファイルが、FindFirstFile と PathMatchSpecExW で検索文字列 "*字*" でマッチするかチェック.
    unsigned n_findfirst     = 0;
    unsigned n_pathmatchspec = 0;
    unsigned bcount = 0;
    WIN32_FIND_DATAW  data;
    for (unsigned i = 0; i < n_wc; ++i) {
        wfname[0] = '*';
        wfname[1] = wchar_t(wbuf[i]);
        wfname[2] = '*';
        wfname[3] = 0;

        memset(&data, 0, sizeof data);
        HANDLE hdl   = FindFirstFileExW(wfname, FindExInfoStandard, &data, FindExSearchLimitToDirectories, 0, 0);
        bool   aflag = (hdl != INVALID_HANDLE_VALUE);
        n_findfirst += aflag;
        if (aflag)
            FindClose(hdl);

        memset(&data, 0, sizeof data);
        bool   bflag = false;
        hdl = FindFirstFileExW(L"*", FindExInfoStandard, &data, FindExSearchLimitToDirectories, 0, 0);
        if (hdl != INVALID_HANDLE_VALUE) {
            ++bcount;
            do {
                bflag = fn_match(wfname, data.cFileName);
                if (bflag)
                    break;
            } while (FindNextFileW(hdl, &data));
            FindClose(hdl);
        }
        n_pathmatchspec += bflag;
     #if 1
        WideCharToMultiByte(65001, 0, wfname, 4, bfname, sizeof bfname, nullptr, nullptr);
        if (!aflag || !bflag)
            printf("%s %d %d %d\n", bfname, aflag, bflag, bcount);
     #endif
    }
    SetCurrentDirectoryW(L"..");

    printf("Char:%u, FindFirst:%u, fn_match:%u-%u=%u\n"
        , n_wc, n_findfirst, bcount, n_pathmatchspec, bcount - n_pathmatchspec);

    SetConsoleOutputCP(sav_cp);
    return 0;
}
