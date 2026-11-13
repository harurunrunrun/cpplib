---
title: Dynamic BitAssign RangeSum (動的ビット代入・区間和)
documentation_of: ../../../../src/structure/segtree/dynamic/dynamic_bitassign_rangesum.hpp
---

長さの大きい 0/1 の列に対して、区間代入と区間和の取得を行うデータ構造である。SIMD とビット列による高速化で定数倍を削っている。ノードは必要な部分だけ疎に確保する。

## API

```cpp
DynamicBit01WSTNoFlip()
explicit DynamicBit01WSTNoFlip(size_t n)
explicit DynamicBit01WSTNoFlip(const vector<int>& values)
void init(size_t n)
size_t size() const
size_t node_count() const
void assign(size_t l, size_t r, bool value)
uint64_t sum(size_t l, size_t r)
bool get(size_t k)
```

- デフォルトコンストラクタは空の列を作る。`DynamicBit01WSTNoFlip(n)` と `init(n)` は長さ `n` の全要素 0 の列を作る。
- `vector` を受け取るコンストラクタは、`values` の非零の要素を 1 として構築する。
- `size()` は列の長さ、`node_count()` は確保済みの疎なノード数を返す。
- `assign(l, r, value)` は半開区間 `[l, r)` を `value` に代入する。
- `sum(l, r)` は半開区間 `[l, r)` にある 1 の個数を、`get(k)` は `k` 番目の値を返す。

## 時間計算量

$N$ を列の長さとする。

- デフォルトコンストラクタ、`DynamicBit01WSTNoFlip(n)`、`init`: $O(1)$
- `DynamicBit01WSTNoFlip(vector)`: $O(P\log N)$（$P$ は 1 の個数）
- `assign`, `sum`, `get`: $O(\log N)$
- `size`, `node_count`: $O(1)$

保存領域は生成したノード数を $K$ として $O(K)$ である。更新 1 回で追加するノードは $O(\log N)$ 個、クエリの追加空間計算量は再帰スタックの $O(\log N)$ である。

## 注意点

- 添字は 0-indexed、区間は半開区間 `[l, r)` で指定する。
- 範囲違反は `assert` で検査する（`NDEBUG` 下では検査しない）。ノードの確保に失敗すると `std::bad_alloc` を送出する。
