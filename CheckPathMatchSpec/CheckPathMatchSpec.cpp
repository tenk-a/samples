// PathMatchSpecExW でマッチしない文字数を求める.
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <string.h>

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

    // 作業ディレクトリ作成.
    wchar_t const* wkdir = L"work";
    CreateDirectoryW(wkdir, 0);
    SetCurrentDirectoryW(wkdir);

    // 抽出した文字ごとのファイルを作成.
    unsigned n_file = 0;
    wchar_t  wfname[16] = {0};
    char     bfname[64] = {0};
    for (unsigned i = 0; i < n_wc; ++i) {
        wfname[0] = wchar_t(wbuf[i]);
        wfname[1] = L'.';
        wfname[2] = L't';
        wfname[3] = 0;
        HANDLE hdl = CreateFileW(wfname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hdl != INVALID_HANDLE_VALUE) {
            WideCharToMultiByte(65001, 0, wfname, 4, bfname, sizeof bfname, nullptr, nullptr);
            WriteFile(hdl, bfname, strlen(bfname), nullptr, 0);
            CloseHandle(hdl);
            ++n_file;
        }
    }

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
                bflag = (PathMatchSpecExW(data.cFileName, wfname, PMSF_NORMAL) == S_OK);
                if (bflag)
                    break;
            } while (FindNextFileW(hdl, &data));
            FindClose(hdl);
        }
        n_pathmatchspec += bflag;
     #if 0
        WideCharToMultiByte(65001, 0, wfname, 4, bfname, sizeof bfname, nullptr, nullptr);
        if (!aflag || !bflag)
            printf("%s %d %d %d\n", bfname, aflag, bflag, bcount);
     #endif
    }
    SetCurrentDirectoryW(L"..");

    printf("Char:%u, File:%u, FindFirst:%u, PathMatchSpecEx:%u-%u=%u\n"
        , n_wc, n_file, n_findfirst, bcount, n_pathmatchspec, bcount - n_pathmatchspec);

    SetConsoleOutputCP(sav_cp);
    return 0;
}
