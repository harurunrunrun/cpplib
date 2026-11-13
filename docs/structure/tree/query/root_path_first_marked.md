---
title: Root Path First Marked (根パス先頭マーク頂点) [QTREE3]
documentation_of: ../../../../src/structure/tree/query/root_path_first_marked.hpp
---

頂点のマークを反転し、根から指定した頂点へのパス上で、根に最も近いマーク頂点を返す。
HL 分解の位置をマーク頂点の順序付き集合（`std::set`）で管理する。

## API

### `RootPathFirstMarked<MAX_SIZE>(n)`

頂点数 `n` で、全頂点が未マークの構造を作る。`MAX_SIZE` は省略でき、デフォルト値 `-1` では上限を検査しない。時間計算量は $O(n)$ である。

### `add_edge(left, right)`

無向辺を追加する。時間計算量はならし $O(1)$ である。

### `build(root = 0)`

根 `root` の HL 分解を構築する。時間計算量は $O(n)$ である。

### `toggle(vertex)`

マークを反転し、反転後にマークされていれば `true` を返す。時間計算量は $O(\log n)$ である。

### `first_marked(vertex)`

根から `vertex` へのパス上で最初のマーク頂点を返す。存在しなければ `-1` を返す。時間計算量は $O(\log^2 n)$ である。

## 注意点

- `0 <= n` で、固定長版では `n <= MAX_SIZE` でなければならない。違反時は `runtime_error` を送出する。辺集合は連結な木でなければならない。
- `build` 前の `toggle` / `first_marked` では `runtime_error` を送出する。範囲外の頂点の検査は内部の HL 分解に依存する。
- 空間計算量は $O(n)$ である。容量を指定した版（`RootPathFirstMarked<N>`）と省略した版（`RootPathFirstMarked<>`）は同じヘッダで利用できる。
