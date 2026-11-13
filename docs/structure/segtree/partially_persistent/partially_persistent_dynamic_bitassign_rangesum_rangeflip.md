---
title: Partially Persistent Dynamic BitAssign RangeSum RangeFlip (部分永続動的ビット代入・区間和・区間反転)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大なビット列に対して、部分永続な区間代入・区間反転・区間和を扱う。

このヘッダは include 互換用で、`partially_persistent_bitassign_rangesum_rangeflip.hpp` を読み込むだけである。`PartiallyPersistentDynamicBitAssignRangeSumRangeFlip` は `PartiallyPersistentBitAssignRangeSumRangeFlip` の別名テンプレートで、テンプレート引数 `MAX_SIZE`, `MAX_NODES`, `MAX_VERSIONS`, `MAX_CHANGES`（すべてデフォルト $-1$）もそのまま受け取る。API と契約は `PartiallyPersistentBitAssignRangeSumRangeFlip` と同じである。

## API

- コンストラクタ `PartiallyPersistentDynamicBitAssignRangeSumRangeFlip<...>(n)`: 長さ $n$ の全 0 列を version 0 として作る。
- `size()`, `versions()`, `latest_version()`, `nodes_used()`, `changes_used()`: 長さ・version 数・最新 version・使用ノード数・使用履歴数を返す。
- `fork(version)`: 最新版と同じ内容の新しい version を作って返す。
- `assign(version, l, r, value)`: 半開区間 $[l, r)$ を `value` に代入した新しい version を返す。
- `flip(version, l, r)`: 半開区間 $[l, r)$ を反転した新しい version を返す。
- `set_one(version, l, r)`, `set(version, l, r)`: 半開区間 $[l, r)$ を 1 に上書きした新しい version を返す（`set` は `set_one` と同じ）。
- 上の更新関数はすべて `version` を省略でき、そのときは `latest_version()` を更新元とする。
- `sum(version, l, r)`: 指定 version の半開区間 $[l, r)$ の 1 の個数を返す。
- `get(version, k)`: 指定 version の位置 $k$ のビットを `bool` で返す。

## 時間計算量

$N = \texttt{MAX_SIZE}$（容量省略時は $n$）とする。

- コンストラクタ: 固定容量版は $O(\texttt{MAX_NODES}+\texttt{MAX_VERSIONS}+\texttt{MAX_CHANGES})$、容量省略版は $O(1)$
- 更新と最新版のクエリ: $O(\log N)$
- 過去 version のクエリ: $O(\log N \log C)$（$C$ は `changes_used()`）
- 状態取得関数: $O(1)$

更新 1 回あたり $O(\log N)$ 個のノードと履歴を消費し、クエリは $O(\log N)$ の再帰スタックを使う。

## 注意点

- 更新元にできるのは `latest_version()` だけで、過去の version はクエリにしか使えない。
- 半開区間・位置・version の範囲外、最新版以外からの更新、各固定容量の超過では `std::runtime_error` を送出する。$n \le 0$ も `std::runtime_error` になる。
- テンプレート引数をすべて省略した場合は、ノード・version・履歴の容量が要素の増加に応じて伸びる。
