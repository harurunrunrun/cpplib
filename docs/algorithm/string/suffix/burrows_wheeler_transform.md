---
title: Burrows-Wheeler Transform (Burrows-Wheeler変換)
documentation_of: ../../../../src/algorithm/string/suffix/burrows_wheeler_transform.hpp
---

文字列の巡回シフトを辞書順に並べた最終列と、元文字列の行番号を相互変換する。
終端記号を追加しないため、`primary_index` も保持する。

## API

```cpp
BurrowsWheelerTransformResult burrows_wheeler_transform(
    const std::string& text
)
std::string inverse_burrows_wheeler_transform(
    const std::string& transformed, int primary_index
)
```

結果型は `transformed` と `primary_index` を持つ。逆変換はLF-mappingを用いる。

## API別の時間計算量・空間計算量

$N$ を文字列長とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `burrows_wheeler_transform` | $O(N\log N+256)$ | $O(N+256)$ |
| `inverse_burrows_wheeler_transform` | $O(N+256)$ | $O(N+256)$ |

空文字列の `primary_index` は0である。不正な行番号では例外を送出する。

## 注意点

変換列だけでは巡回同値な文字列を区別できないため、行番号も保存する必要がある。
