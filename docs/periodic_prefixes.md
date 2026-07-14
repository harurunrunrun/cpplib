---
title: Periodic Prefixes (周期的接頭辞) [PERIOD]
documentation_of: ../src/algorithm/string/periodic_prefixes.hpp
---

列の各接頭辞について、同じブロックを2回以上連結して表せるものを列挙する。
内部では `prefix_function` を再利用し、最短ブロック長から最大反復回数を求める。

# periodic_prefixes

```cpp
template<class Sequence>
vector<pair<int, int>> periodic_prefixes(const Sequence& sequence)
```

周期的な接頭辞を `(prefix_length, repetition_count)` の組で返す。
`prefix_length` は接頭辞の長さ、`repetition_count` は同じブロックを連結する最大回数である。
結果は `prefix_length` の昇順になる。

例えば `sequence = "ababab"` なら `(4, 2), (6, 3)` を返す。

## API別の時間計算量・空間計算量

$N$ を入力列の長さ、$K$ を返す組の個数とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `periodic_prefixes(sequence)` | $O(N)$ | $O(N+K)$ |

要素の等値比較1回の計算量を $O(1)$ とする。$O(N)$ の追加領域は接頭辞関数に使用する。

## 注意点

- `Sequence` は `size()`、0-indexedのランダムアクセス、要素の等値比較に対応する必要がある。
- 長さ0または1の列では空の結果を返す。
- 反復回数1の接頭辞は返さない。
- 組の第1要素は0-indexed位置ではなく接頭辞の長さである。
- 反復回数は最短ブロックを用いたときの最大値である。
