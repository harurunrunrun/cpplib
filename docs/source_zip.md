---
title: Integer Array Encoding for Source Code (ソースコード埋め込み用整数配列圧縮)
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

## API別計算量

$N$ を元の整数列長、$E$ を符号化文字列長、$R$ をrun数とする。整数型のbit幅は固定とする。

| API | 時間計算量 | 出力領域 |
| --- | --- | --- |
| `source_zip(values)` | $O(N+E)$ | $O(E)$ |
| `source_unzip(encoded)` | $O(E+N)$ | $O(N)$ |

`source_zip` は $N$ 要素から $R$ runを作り、varint列とBase64文字列を列挙する。`source_unzip` は全 $E$ 文字を検証してから $N$ 要素を復元する。
