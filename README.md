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

```sh
make docs
```

testを実行した後、生成したJekyll sourceを `.competitive-verifier/_jekyll` に配置する。
