---
title: Deterministic Selection / Majority Vote (決定的選択・多数決法)
documentation_of: ../src/algorithm/other/sequence/selection.hpp
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

範囲を `compare` 順に並べたときの0-indexed `rank` 番目と等しい要素を指すiteratorを返す。範囲はin-placeに並べ替えるが、全体をsortはしない。

5要素ごとの中央値からmedian of mediansを選び、pivot未満・同値・pivot超の3区間へ分割する。同値要素をまとめて処理するため、全要素が等しい場合も線形時間。

要素型はcopy constructible、`compare` はstrict weak orderingでなければならない。空範囲または範囲外の `rank` は `out_of_range`、反転したiterator範囲は `invalid_argument`。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `median_of_medians_select` | worst-case $O(N)$ | 再帰stack $O(\log N)$ |

## `boyer_moore_majority`

```cpp
template<forward_iterator Iterator>
optional<iter_value_t<Iterator>> boyer_moore_majority(
    Iterator first,
    Iterator last
)
```

範囲の半数より多く現れる要素を返す。存在しない場合と空範囲では `nullopt`。

1回目の走査でBoyer--Moore majority voteにより候補を絞り、2回目で出現数を検証する。要素型はcopy constructibleかつ等値比較可能で、範囲は複数回走査できる必要がある。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `boyer_moore_majority` | $O(N)$ | $O(1)$ |

## 注意点

空入力、退化入力、範囲外のindexと容量超過の扱いは各APIの説明に従う。参照・iterator・callbackを受け渡すAPIでは、変更可否とobjectの寿命を守る。
