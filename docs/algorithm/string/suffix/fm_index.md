---
title: FM-Index (FM索引)
documentation_of: ../../../../src/algorithm/string/suffix/fm_index.hpp
---

BWT列の各byteをbit vectorで保持し、後方検索でパターンの接尾辞配列区間を求める。

## API

```cpp
FMIndex(const std::string& text)
int rank(int symbol, int end) const
std::pair<int, int> equal_range(std::string_view pattern) const
int count(std::string_view pattern) const
std::vector<int> locate(std::string_view pattern) const
```

`rank(symbol,end)` はBWTの `[0,end)` にある記号数を返す。`equal_range` は
接尾辞配列上の半開区間、`locate` はその開始位置を返す。空パターンは終端位置を含む
$N+1$ 箇所に一致する。

## API別の時間計算量・空間計算量

$N=|text|,\ M=|pattern|,\ K$ を一致件数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N+256\lceil N/64\rceil)$ | $O(N+256\lceil N/64\rceil)$ |
| `rank` | $O(1)$ | $O(1)$ |
| `equal_range`, `count` | $O(M)$ | $O(1)$ |
| `locate` | $O(M+K)$ | $O(K)$ |

## 注意点

`locate` の返り値は接尾辞配列順であり、位置の昇順とは限らない。
