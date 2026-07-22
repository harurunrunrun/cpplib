---
title: Periodic Prefixes (周期的接頭辞) [PERIOD]
documentation_of: ../src/algorithm/string/automata/periodic_prefixes.hpp
---

列の各接頭辞について、同じブロックを2回以上連結して表せるものを列挙する。

## API

```cpp
template<class Sequence>
vector<pair<int, int>> periodic_prefixes(const Sequence& sequence)
```

## 引数・戻り値

- `sequence`: 周期的接頭辞を列挙する入力列。
- 戻り値: `(prefix_length, repetition_count)` の列。
  `prefix_length` は接頭辞の長さ、`repetition_count` は同じ非空ブロックを
  連結できる最大回数である。結果は `prefix_length` の昇順で、反復回数1は含まない。
  長さ0または1の入力では空vectorを返す。

## API別の時間計算量・空間計算量

$N$ を入力列の長さ、$K$ を返す組の個数とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `periodic_prefixes(sequence)` | $O(N)$ | 追加領域 $O(N)$、出力 $O(K)$ |

要素の等値比較1回の計算量を $O(1)$ とする。

## 注意点

- `Sequence` は `size()`、0-indexedのランダムアクセス、要素の等値比較に対応する必要がある。
- 長さ0または1の列では空の結果を返す。
- 反復回数1の接頭辞は返さない。
- 組の第1要素は0-indexed位置ではなく接頭辞の長さである。
