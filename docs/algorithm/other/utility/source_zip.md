---
title: Integer Array Encoding for Source Code (ソースコード埋め込み用整数配列圧縮)
documentation_of: ../../../../src/algorithm/other/utility/source_zip.hpp
---

整数配列をソースコードへ埋め込める文字列に変換する。

```cpp
template<class T>
string source_zip(const vector<T>& values)

template<class T>
vector<T> source_unzip(string_view encoded)
```

同じ値の連続部分をランレングス圧縮し、整数を可変長整数（varint、符号付きは zigzag 符号化）、全体をパディングなしの URL-safe Base64 で符号化する。生成文字列にエスケープが必要な文字は含まれない。

`T` は `bool` 以外の 64 bit 以下の整数型とする。

## 時間計算量・空間計算量

配列長と生成データ長に線形である。

$N$ を元の整数列長、$E$ を符号化文字列長、$R$ をラン数とする。整数型のビット幅は固定とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `source_zip(values)` | $O(N+E)$ | $O(E)$ |
| `source_unzip(encoded)` | $O(E+N)$ | $O(E+N)$ |

`source_zip` は $N$ 要素から $R$ 個のランを作り、varint の列と Base64 文字列を列挙する。`source_unzip` は全 $E$ 文字を検証してから $N$ 要素を復元する。

## 注意点

- 符号化と復号には同じ整数型 `T` を指定する。
- 復号する文字列はこの API が生成するパディングなし URL-safe Base64 形式でなければならない。破損、オーバーフロー、余分なデータには `runtime_error` を送出する。

## 内部API

`source_zip_internal` 名前空間には `alphabet`、`append_varuint`、`read_varuint`、`encode_base64`、`base64_value`、`decode_base64`、`encode_integer`、`decode_integer` がある。上位 2 関数の実装用であり、互換性は保証しない。

| 内部API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `alphabet`, `base64_value`, `encode_integer`, `decode_integer` | $O(1)$ | $O(1)$ |
| `append_varuint`, `read_varuint` | 固定ビット幅では $O(1)$ | $O(1)$ |
| `encode_base64(bytes)` | $O(B)$ | 戻り値 $O(B)$ |
| `decode_base64(text)` | $O(E)$ | 戻り値 $O(E)$ |

`read_varuint` は `position` を次のバイトへ進める。内部の復号関数は不正な文字、非零のパディングビット、途中での打ち切り、整数の範囲外を `runtime_error` で通知する。
