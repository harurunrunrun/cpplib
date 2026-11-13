---
title: Static Perfect Hash Set (静的完全ハッシュ集合)
documentation_of: ../../../src/structure/hash/perfect_hash_set.hpp
---

FKS の 2 段ハッシュ法により、構築後の衝突をなくした静的な集合。

## API

```cpp
template<class Key, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class PerfectHashSet;

PerfectHashSet()
explicit PerfectHashSet(const vector<Key>& keys)
void build(const vector<Key>& keys)
bool empty() const
size_t size() const
bool contains(const Key& key) const
```

コンストラクタと `build` は、等しいキーを 1 個にまとめてから構築する。
1 段目はバケットのサイズの 2 乗和が線形（$8N$ 以下）になるまで乱択ハッシュ（multiply-shift 族）を選び直し、続いて 2 段目の各表を衝突がなくなるまで選び直す。
`contains` は 1 段目と 2 段目のスロットをちょうど 1 つずつ読み、候補を `Equal` で確認する。

## 時間計算量

入力の長さを $I$、相異なるキーの数を $N$ とし、`Hash` と `Equal` は最悪 $O(1)$、基底ハッシュはキーを分離できるものとする。

- 構築（コンストラクタ、`build`）: 期待 $O(I+N)$
- `contains`: 最悪 $O(1)$
- `empty`、`size`: $O(1)$

再選択のループは回数で打ち切らないが、各試行は一定の確率で成功するので、期待 $O(1)$ 回かつ確率 $1$ で終了する。

## 空間計算量

- 構築中の一時領域: $O(I+N)$
- 構築後: 期待 $O(N)$

## 注意点

- `Hash` と `Equal` は通常のハッシュの規約を満たし、さらに格納する等しくないキーどうしの基底ハッシュ値（`Hash` の値）はすべて異なる必要がある。
- 有限回で打ち切る遅いフォールバックは持たない。等しくない 2 つのキーの `Hash` の値が一致すると、`build` は `std::invalid_argument` を送出する（たとえば定数のハッシュに複数の異なるキーを渡した場合）。
- 構築に失敗した場合も、それ以前に構築した集合は保たれる。
