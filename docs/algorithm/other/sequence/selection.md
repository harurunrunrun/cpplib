---
title: Deterministic Selection / Majority Vote (決定的選択・多数決法)
documentation_of: ../../../../src/algorithm/other/sequence/selection.hpp
---

## `median_of_medians_select`

```cpp
template<random_access_iterator Iterator, class Compare = less<>>
Iterator median_of_medians_select(
    Iterator first,
    Iterator last,
    size_t rank,
    Compare compare = {}
)
```

範囲を `compare` の順に並べたときの 0-indexed で `rank` 番目と等しい要素を指すイテレータを返す。範囲はインプレースに並べ替えるが、全体をソートはしない。

5 要素ごとの中央値から median of medians を選び、ピボット未満・ピボットと同値・ピボット超の 3 区間へ分割する。同値な要素をまとめて処理するため、全要素が等しい場合も線形時間である。

要素型はコピー構築可能で、`compare` は狭義弱順序でなければならない。空範囲または範囲外の `rank` では `out_of_range` を、逆転したイテレータ範囲では `invalid_argument` を送出する。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `median_of_medians_select` | 最悪 $O(N)$ | 再帰スタック $O(\log N)$ |

## `boyer_moore_majority`

```cpp
template<forward_iterator Iterator>
optional<iter_value_t<Iterator>> boyer_moore_majority(
    Iterator first,
    Iterator last
)
```

範囲内で半数より多く現れる要素を返す。存在しない場合と空範囲では `nullopt` を返す。

1 回目の走査で Boyer–Moore の過半数投票により候補を絞り、2 回目の走査で出現数を検証する。要素型はコピー可能かつ等値比較可能で、範囲は複数回走査できなければならない。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `boyer_moore_majority` | $O(N)$ | $O(1)$ |

## 注意点

- `median_of_medians_select` は範囲の要素の順序を変更する。返り値のイテレータは範囲内の要素を指す。
- `compare` や要素の比較が送出した例外はそのまま伝わる。
