https://zenn.dev/tenka/articles/use_vcpkg_and_cmake
https://zenn.dev/tenka/articles/use_vcpkg_and_vs

のサンプル.

CMakeLists.txt      cmake ビルド用.
vcpkg.json          マニフェスト・モード用ファイル.
                    クラシック・モードでビルドする場合は、vcpkg.json を削除のこと.
src/                ソース
build/              cmake ビルドでのビルド作業用ディレクトリ.
build_vc143vcpkg/   VS統合用のサンプル.
build_vc143env/     VS統合を使わず環境変数 VCPKG_ROOT を参照するサンプル.
                    予め システムのプロパティ→環境変数 等で設定しておくこと.

詳しくは、リンクの頁をみてください.
