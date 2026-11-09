---
title: Burrows-Wheeler Transform (Burrows-Wheeler 変換)
documentation_of: ../../../../src/algorithm/string/suffix/burrows_wheeler_transform.hpp
---

文字列の巡回シフトを辞書順に並べたときの最終列（変換列）と、元の文字列がある行番号を求める変換、およびその逆変換を行う。
終端記号を追加しないため、行番号 `primary_index` も併せて保持する。

## API

```cpp
BurrowsWheelerTransformResult burrows_wheeler_transform(
    const std::string& text
)
std::string inverse_burrows_wheeler_transform(
    const std::string& transformed, int primary_index
)
```

結果型 `BurrowsWheelerTransformResult` はメンバ `std::string transformed` と `int primary_index` を持つ。
順変換は巡回シフトのダブリングと計数ソートで行い、逆変換は LF-mapping を用いる。
各文字は `unsigned char` として扱い、全 256 種のバイトを区別する。

## API 別の時間計算量・空間計算量

$N$ を文字列長とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `burrows_wheeler_transform` | $O(N\log N+256)$ | $O(N+256)$ |
| `inverse_burrows_wheeler_transform` | $O(N+256)$ | $O(N+256)$ |

空文字列に対する `primary_index` は `0` である。
逆変換で `primary_index` が範囲外（空文字列では `0` 以外、それ以外では $[0, N)$ の外）の場合は `std::runtime_error` を送出する。

## 注意点

変換列だけでは巡回同値な文字列を区別できないため、行番号も保存する必要がある。
