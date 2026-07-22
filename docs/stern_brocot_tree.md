---
title: Stern–Brocot Tree (スターン・ブロコット木) [stern_brocot_tree]
documentation_of: ../src/algorithm/math/combinatorics/stern_brocot_tree.hpp
---

正の既約分数をStern--Brocot Treeの頂点として扱う。
根は `1/1`、左端と右端の番兵はそれぞれ `0/1` と `1/0` である。

```cpp
using SBT = math::SternBrocotTree<long long, 128>;
using Fraction = SBT::Fraction;
```

第2 template引数 `MAX_RUNS` は、根から頂点までのpathをrun-length encodingしたときの
run数上限である。pathは `std::array` に保持され、動的確保を行わない。

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

`Fraction` は通常の分数、`WideFraction` は `search` のpredicateへ渡す
`__int128_t` 成分の分数である。`Run::length` とdepthは `uint64_t`。

`Path` は次の操作を持つ。

| API | 説明 | 時間計算量 |
| --- | --- | --- |
| `capacity()` | run数上限 `MAX_RUNS` | $O(1)$ |
| `size()` / `empty()` | 現在のrun数 | $O(1)$ |
| `begin()` / `end()` | run列のiterator | $O(1)$ |
| `operator[](i)` | `i` 番目のrun | $O(1)$ |
| `clear()` | pathを空にする | $O(1)$ |
| `push_back(dir,len)` | runを末尾へ追加。同方向なら結合 | $O(1)$ |
| `depth()` | 全run長の和 | $O(R)$ |

`len == 0`、run数超過、run長またはdepthのoverflowは例外になる。

## pathと頂点

| API | 説明 | 時間計算量 |
| --- | --- | --- |
| `is_node(f)` | `f` が正の既約分数か判定 | $O(\log \min(a,b))$ |
| `reduce(f)` | 正の分数を既約化 | $O(\log \min(a,b))$ |
| `compare(f,g)` | 分数の大小を `-1,0,1` で返す | $O(1)$ |
| `encode_path(f)` | `1/1` から `f` までのRLE path | $O(R)$ |
| `from_path(path)` / `decode_path(path)` | RLE pathの頂点 | $O(R)$ |
| `depth(f)` | 頂点のdepth | $O(R)$ |
| `ancestor(f,k)` | depth `k` の祖先。存在しなければ `nullopt` | $O(R)$ |
| `lca(f,g)` | 2頂点のLCA | $O(R_f+R_g)$ |
| `is_ancestor(a,f)` | `a` が `f` の祖先か判定 | $O(R_a+R_f)$ |
| `range(f)` | `f` の部分木を含む開区間の下限・上限 | $O(R)$ |
| `descend(f,dir,k)` | 同じ方向へ `k` edge降りた頂点 | $O(R)$ |

`range` は端で `0/1` と `1/0` を返すことがある。`compare` はこの2つも扱える。
`encode_path`、祖先・LCAなど頂点を要求するAPIには正の既約分数を渡す。

## 区間中の最も単純な分数

```cpp
Fraction simplest_in_closed_interval(Fraction lower, Fraction upper);
Fraction simplest_in_open_interval(Fraction lower, Fraction upper);
```

区間内でStern--Brocot Tree上のdepthが最小の頂点を返す。
閉区間版は端点の順序を入れ替えてもよい。開区間版は `lower < upper` が必要である。

両端が異なる部分木にある場合はLCAが答えになる。一方がLCAの場合も、
端点自身や区間外の祖先を返さず、RLE pathの分岐から最小depthの内部頂点を構成する。
例えば `(1/1, 3/2)` では `4/3` を返す。

両APIの時間計算量は $O(R_l+R_u)$、空間計算量（追加領域）は固定長の `Path` 数個である。

## 有界な分数の探索

```cpp
Fraction search(Int max_component, Predicate is_less_than_target);
```

分子・分母がともに `max_component` 以下である未知の正の既約分数 `x` を探索する。
predicateは任意の正の分数 `v` に対して `v < x` のときだけ `true` を返す必要がある。
predicateにはoverflowを避けるため `WideFraction` が渡される。

RLE runごとに移動量を指数探索・二分探索する。`x` のrun数を $R$ とすると、
predicate呼出し回数は $O(R\log(max\_component+1))$、固定長path以外の空間計算量（追加領域）は $O(1)$。
`max_component <= 0`、predicateが単調でない場合、または範囲内に境界となる頂点がない場合の
利用は契約外である。

## 注意点

- `Int` は64bit以下の符号付き整数型。
- 計算途中は `__int128_t` を使い、返却する分子・分母が `Int` に収まらなければ
  `overflow_error` を送出する。
- 不正な分数・空の開区間・0以下の探索上限は `invalid_argument`。
- pathのrun数が `MAX_RUNS` を超える場合は `length_error`。
- path indexが範囲外なら `out_of_range`。
