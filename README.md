# cpplib
under construction

## Local verification

WSLまたはLinuxのリポジトリルートで実行する。

```sh
make verify
```

初回実行時に `.competitive-verifier/venv` を作り、competitive-verifierをインストールする。

新規ファイルはcompetitive-verifierが列挙できるよう、実行前に `git add` しておく。

## Local documentation

Ubuntu/WSLでは最初にRubyとBundlerをインストールする。

```sh
sudo apt install ruby-all-dev ruby-bundler
make docs
```

`make docs` はtestを実行せず、HTMLを `.competitive-verifier/site` に生成する。直前の `make verify` の結果があれば、検証状況もHTMLへ反映する。ブラウザで `.competitive-verifier/site/index.html` を開けば確認できる。

ローカルサーバーで確認する場合は次を実行し、<http://127.0.0.1:4000>を開く。

```sh
make docs-serve
```

Jekyll sourceのみ生成する場合は `make docs-source` を実行する。

## Test layout

- `test/onlinejudge/*.test.cpp`: online judgeを使うverification
- `test/standalone/*.test.cpp`: 自己完結する独自テスト
- `test/generator/<name>/`: 独自テストケースのgenerator
- `test/checker/<name>/`: 独自checker
- `test/tools/`: verification用の補助ツール

独自テストは `competitive-verifier: STANDALONE` を指定し、`make verify` から実行する。generatorとcheckerもテスト資産として残し、テスト本体と一緒にコミットする。
