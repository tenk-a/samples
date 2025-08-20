curses を用いたミニゲーム・サンプルとしてテトリスもどき
https://zenn.dev/tenka/articles/samplegame_using_curses

curses_otige2 フォルダは、
curses_otige フォルダの cons.h を timeout(50) から、
nodelay の待ち無し入力＆別途時間管理 に変更したバージョン.
現在時間取得がターゲットによって違うため少し煩雑でソース5割増.

※watcom-dos32 等たまに初回ビルドで失敗(libリンク漏)することがあるが、
　継続で再度ビルドするとリンクされるので吉としてます。
