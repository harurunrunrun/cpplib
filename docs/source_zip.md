---
title: Source Zip
documentation_of: ../src/algorithm/other/source_zip.hpp
---

整数配列をソースコードへ埋め込める文字列に変換する。

```cpp
string source_zip(const vector<T>& values)
vector<T> source_unzip<T>(string_view encoded)
```

同じ値の連続部分をrun-length圧縮し、整数をvarint、全体をURL-safe Base64で符号化する。生成文字列にエスケープが必要な文字は含まれない。

`T` は `bool` 以外の64bit以下の整数型とする。復号時に壊れた文字列、オーバーフロー、余分なデータを検出した場合は例外を送出する。

## 計算量

配列長と生成データ長に線形。
