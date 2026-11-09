---
title: Stern–Brocot Tree (スターン・ブロコット木) [stern_brocot_tree]
documentation_of: ../../../../src/algorithm/math/combinatorics/stern_brocot_tree.hpp
---

正の既約分数を Stern–Brocot 木の頂点として扱う。根は `1/1`、左端と右端の番兵はそれぞれ `0/1` と `1/0` である。

```cpp
using SBT = math::SternBrocotTree<long long, 128>;  // 固定長版
using Fraction = SBT::Fraction;

math::SternBrocotTree<> dynamic;                    // 可変長版（Int = long long）
```

第 2 テンプレート引数 `MAX_RUNS` は、根から頂点までの経路をランレングス圧縮（RLE）したときの run 数の上限である。指定すると経路を `std::array` に保持し、動的確保を行わない。省略する（`-1`）と `std::vector` を使う可変長版になり、run 数の上限はなくなる。

以下、経路の run 数を $R$ とする。

## 型

```cpp
Fraction{numerator, denominator}
WideFraction{numerator, denominator}
Direction::Left
Direction::Right
Run{direction, length}
Path
Range{lower, upper}
```

`Fraction` は通常の分数、`WideFraction` は `search` の述語に渡される `__int128_t` 成分の分数である。`Run::length` と深さは `uint64_t` で表す。

`Path` は次の操作を持つ。

| API | 説明 | 時間計算量 |
| --- | --- | --- |
| `capacity()` | run 数の上限 `MAX_RUNS`（可変長版では `-1`） | $O(1)$ |
| `size()` / `empty()` | 現在の run 数 | $O(1)$ |
| `begin()` / `end()` | run 列のイテレータ | $O(1)$ |
| `operator[](i)` | `i` 番目の run | $O(1)$ |
| `clear()` | 経路を空にする | $O(1)$ |
| `push_back(dir, len)` | run を末尾へ追加する。直前と同じ方向なら結合する | $O(1)$（可変長版は償却） |
| `depth()` | 全 run の長さの和 | $O(R)$ |

`len == 0` なら `invalid_argument`、run 数の上限超過なら `length_error`、run 長または深さのオーバーフローなら `overflow_error` を送出する。

## 経路と頂点

| API | 説明 | 時間計算量 |
| --- | --- | --- |
| `is_node(f)` | `f` が正の既約分数か判定する | $O(\log \min(a,b))$ |
| `reduce(f)` | 正の分数を既約化する | $O(\log \min(a,b))$ |
| `compare(f, g)` | 分数の大小を `-1`, `0`, `1` で返す | $O(1)$ |
| `encode_path(f)` | `1/1` から `f` までの RLE 経路 | $O(R)$ |
| `from_path(path)` / `decode_path(path)` | RLE 経路が指す頂点 | $O(R)$ |
| `depth(f)` | 頂点の深さ | $O(R)$ |
| `ancestor(f, k)` | 深さ `k` の祖先。存在しなければ `nullopt` | $O(R)$ |
| `lca(f, g)` | 2 頂点の LCA | $O(R_f+R_g)$ |
| `is_ancestor(a, f)` | `a` が `f` の祖先か判定する | $O(R_a+R_f)$ |
| `range(f)` | `f` の部分木を含む開区間の下限・上限 | $O(R)$ |
| `descend(f, dir, k)` | 方向 `dir` へ `k` 辺（デフォルト 1）降りた頂点 | $O(R)$ |

`range` は端で `0/1` や `1/0` を返すことがある。`compare` はこの 2 つも扱える。`encode_path`、祖先・LCA など頂点を要求する API には正の既約分数を渡す。

## 区間中の最も単純な分数

```cpp
Fraction simplest_in_closed_interval(Fraction lower, Fraction upper);
Fraction simplest_in_open_interval(Fraction lower, Fraction upper);
```

区間内で Stern–Brocot 木上の深さが最小の頂点を返す。端点は既約化してから扱う。閉区間版は端点の順序を入れ替えてもよい（両端点の LCA を返す）。開区間版は `lower < upper` が必要である。

開区間版では、両端が異なる部分木にある場合は LCA が答えになる。一方の端点が LCA である場合も、端点自身や区間外の祖先は返さず、RLE 経路の分岐から深さ最小の内部の頂点を構成する。例えば `(1/1, 3/2)` では `4/3` を返す。

両 API の時間計算量は $O(R_l+R_u)$、空間計算量（追加領域）は `Path` 数個分である。

## 有界な分数の探索

```cpp
Fraction search(Int max_component, Predicate is_less_than_target);
```

分子・分母がともに `max_component` 以下である未知の正の既約分数 $x$ を探索する。述語は任意の正の分数 $v$ に対して、$v < x$ のときに限り `true` を返す必要がある。述語にはオーバーフローを避けるため `WideFraction` が渡される。

RLE の run ごとに移動量を指数探索・二分探索で決める。$x$ の run 数を $R$、`max_component` を $M$ とすると、述語の呼び出し回数は $O(R \log (M+1))$、`Path` 以外の追加空間計算量は $O(1)$ である。`max_component <= 0` なら `invalid_argument` を送出する。述語が単調でない場合や、範囲内に境界となる頂点がない場合の動作は保証しない。

## 注意点

- `Int` は 64 bit 以下の符号付き整数型。
- 計算途中は `__int128_t` を使い、返す分子・分母が `Int` に収まらなければ `overflow_error` を送出する。
- 不正な分数・空の開区間・0 以下の探索上限では `invalid_argument` を送出する。
- 固定長版で経路の run 数が `MAX_RUNS` を超える場合は `length_error` を送出する。
- `Path` の添字が範囲外なら `out_of_range` を送出する。
