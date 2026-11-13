---
title: Interval Set (区間集合)
documentation_of: ../../../../src/algorithm/range/interval/interval_set.hpp
---

半開区間 $[l, r)$ の和集合を、互いに素な区間の集合として管理する。接する区間は併合される。

## コンストラクタ

```cpp
IntervalSet<T> intervals
```

## 関数

```cpp
intervals.add(l, r)
intervals.erase(l, r)
intervals.contains(x)
intervals.covered(l, r)
intervals.mex(x)
intervals.intervals()
```

`mex(x)` は `x` 以上で集合に含まれない最小の値を返す。

## 時間計算量

$M$ を現在の互いに素な区間数、$K$ をその呼出しで削除する区間数とする。

| API | 時間計算量 |
| --- | --- |
| `IntervalSet()` | $O(1)$ |
| `empty()` / `size()` | $O(1)$ |
| `clear()` | $O(M)$ |
| `intervals()` | $O(1)$（`const map&` の返却） |
| `contains(x)` / `covered(l,r)` / `mex(x)` | $O(\log M)$ |
| `add(l,r)` | $O(\log M+K)$ |
| `erase(l,r)` | $O(\log M+K)$ |

`intervals()` 自体は列挙しない。返された `std::map` を全走査する場合は別途 $O(M)$ かかる。保持領域は $O(M)$。

## WeightedIntervalSet

区間ノードごとにモノイドの値を 1 つ持つ区間集合。

```cpp
struct ConcatMonoid{
    using S = string;
    S op(S a, const S& b) const{ return a += b; }
    S e() const{ return {}; }
};
constexpr ConcatMonoid concat_monoid{};

WeightedIntervalSet<concat_monoid, long long> weighted;
```

第 1 テンプレート引数は既存のデータ構造と同じくモノイドのオブジェクト、第 2 引数は座標型で、デフォルトは `long long` である。公開型は値型 `S`、保存ノード `Interval{right, value}`、列挙用の `Entry{left, right, value}`、内部の map の型 `map_type` である。

### 更新と不変条件

```cpp
weighted.set(l, r, value)
weighted.assign(l, r, value)
weighted.erase(l, r)
weighted.clear()
```

`set` と `assign` は同じ操作で、$[l,r)$ と重なる既存区間を `value` の 1 区間へ置き換える。既存区間の途中で境界を作ると、元の値を左右の断片へコピーする。隣接区間は値が `operator==` で等しい場合だけ併合し、異なる値なら接したまま別ノードとして保持する。`erase` も境界で分割してから範囲を取り除く。$l\ge r$ の更新は何もしない。

### 取得と列挙

```cpp
weighted.empty()
weighted.size()
weighted.intervals()
weighted.contains(x)
weighted.covered(l, r)
weighted.mex(x)
weighted.get(x)
weighted.interval(x)
weighted.enumerate()
weighted.enumerate(l, r)
```

- `get(x)` は点 $x$ を含むノードの値を `optional<S>` で返す。
- `interval(x)` はそのノードの元の左右端と値を `optional<Entry>` で返す。
- 引数なし `enumerate` は全ノード、範囲版は $[l,r)$ と交差するノードを左から返す。範囲版も端点を切り詰めず、保存されている元の `left, right` を返す。
- `intervals()` は左端をキー、`Interval` を値とする内部の map への `const&` を返す。
- `covered` と `mex` は、値が異なるため併合されていない隣接ノードも連続した被覆として扱う。

### モノイド積

```cpp
weighted.prod(l, r)
weighted.all_prod()
```

`prod(l,r)` は $[l,r)$ と交差する各保存ノードの値をちょうど 1 回、左端の昇順に `Monoid.op` で積む。区間の長さだけ繰り返したり、部分的な交差を重み付けしたりしない。区間の隙間は飛ばし、交差ノードがない場合と $l\ge r$ では `Monoid.e()` を返す。`all_prod()` は全ノードを同じ順序で積むため、非可換なモノイドでも順序が保存される。

### 計算量

$M$ を現在のノード数、$K$ をその操作で削除・走査・出力するノード数とする。`S` のコピー・ムーブ・比較を $O(1)$ とし、`Monoid.op` と `Monoid.e` の計算量をそれぞれ $C_{op}$、$C_e$ とする。

| API | 時間計算量 |
| --- | --- |
| `WeightedIntervalSet()` | $O(1)$ |
| `empty()` / `size()` | $O(1)$ |
| `clear()` | $O(M)$ |
| `intervals()` | $O(1)$（参照返却。全走査は $O(M)$） |
| `contains(x)` / `get(x)` / `interval(x)` | $O(\log M)$ |
| `set(l,r,value)` / `assign(l,r,value)` | $O(\log M+K)$ |
| `erase(l,r)` | $O(\log M+K)$ |
| `covered(l,r)` / `mex(x)` | $O(\log M+K)$ |
| `enumerate()` | $O(M)$、出力 $O(M)$ |
| `enumerate(l,r)` | $O(\log M+K)$、出力 $O(K)$ |
| `prod(l,r)` | $O(\log M+K C_{op}+C_e)$ |
| `all_prod()` | $O(M C_{op}+C_e)$ |

保持領域は map 本体が $O(M)$、列挙の戻り値が出力ノード数に比例する。

## `IntervalSet` API 詳細

- `empty()` は保存区間がないかを、`size()` は互いに素な保存区間の数を返す。`clear()` はすべて削除する。
- `intervals()` は左端から右端への内部の `std::map<T, T>` の const 参照を返す。参照は次の非 const 操作で無効化され得る。
- `contains(x)` は点 $x$ が集合に含まれるかを、`covered(l,r)` は半開区間全体が 1 つの保存区間に覆われるかを返す。`l>=r` の `covered` は `true`。
- `add(l,r)` は $[l,r)$ を加え、重なる区間と隣接区間を併合する。`erase(l,r)` は範囲を除き、必要なら左右の断片を残す。どちらも `l>=r` では何もしない。
- `mex(x)` は $x$ 以上で集合に含まれない最小の座標を返す。座標型 `T` は順序比較可能でなければならない。

## 注意点

- `IntervalSet` と `WeightedIntervalSet` の更新・取得は、$l\ge r$ の空区間を空集合として扱う（更新は何もしない、`covered` は `true`）。
- `intervals()` が返す参照はオブジェクトの生存中は有効だが、その後の非 const 操作で内容が変わる。
