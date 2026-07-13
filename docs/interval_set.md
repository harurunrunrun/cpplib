---
title: Interval Set
documentation_of: ../src/algorithm/range/interval_set.hpp
---

半開区間 $[l, r)$ の集合。

区間は常に互いに素で、隣接する区間は併合される。

# コンストラクタ

```cpp
IntervalSet<T> intervals
```

# 関数

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

- $O(K \log N)$

`K` は操作で消える区間数。

## API別計算量

$M$ を現在の互いに素な区間数、$K$ をその呼出しで削除する区間数とする。

| API | 時間計算量 |
| --- | --- |
| `IntervalSet()` | $O(1)$ |
| `empty()` / `size()` | $O(1)$（metadata query） |
| `clear()` | $O(M)$ |
| `intervals()` | $O(1)$（`const map&` の返却） |
| `contains(x)` / `covered(l,r)` / `mex(x)` | $O(\log M)$ |
| `add(l,r)` | $O(\log M+K)$ |
| `erase(l,r)` | $O(\log M+K)$ |

`intervals()` 自体は列挙しない。返されたmapを全走査する場合は別途 $O(M)$。保持領域は $O(M)$。

## WeightedIntervalSet

区間nodeごとにMonoid値を1つ持つ区間集合。

```cpp
struct ConcatMonoid{
    using S = string;
    S op(S a, const S& b) const{ return a += b; }
    S e() const{ return {}; }
};
constexpr ConcatMonoid concat_monoid{};

WeightedIntervalSet<concat_monoid, long long> weighted;
```

第1template引数は既存データ構造と同じくMonoid object、第2引数は座標型で、既定値は `long long`。
公開型は値型 `S`、保存node `Interval{right, value}`、列挙用
`Entry{left, right, value}`、map型 `map_type`。

### 更新と不変条件

```cpp
weighted.set(l, r, value)
weighted.assign(l, r, value)
weighted.erase(l, r)
weighted.clear()
```

`set` と `assign` は同じ操作で、$[l,r)$ と重なる既存区間を `value` の1区間へ置き換える。
既存区間の途中で境界を作ると、元の値を左右の断片へコピーする。隣接区間は値が
`operator==` で等しい場合だけ併合し、異なる値なら接したまま別nodeとして保持する。
`erase` も境界で分割してから範囲を取り除く。$l\ge r$ の更新は何もしない。

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

- `get(x)` は点 $x$ を含むnodeの値を `optional<S>` で返す。
- `interval(x)` はそのnodeの元の左右端と値を `optional<Entry>` で返す。
- 引数なし `enumerate` は全node、範囲版は $[l,r)$ と交差するnodeを左から返す。
  範囲版も端点をclipせず、保存されている元の `left, right` を返す。
- `intervals()` は左端をkey、`Interval` を値とする内部mapへの `const&`。
- `covered` と `mex` は、値が異なるため併合されていない隣接nodeも連続した被覆として扱う。

### Monoid積

```cpp
weighted.prod(l, r)
weighted.all_prod()
```

`prod(l,r)` は $[l,r)$ と交差する各保存nodeの値をちょうど1回、左端の昇順に
`Monoid.op` で積む。区間の長さだけ繰り返したり、部分交差を重み付けしたりしない。
gapは飛ばし、交差nodeがない場合と $l\ge r$ では `Monoid.e()` を返す。
`all_prod()` は全nodeを同じ順序で積むため、非可換Monoidでも順序が保存される。

### 計算量

$M$ を現在のnode数、$K$ をその操作で削除・走査・出力するnode数とする。
`S` のコピー・move・比較を $O(1)$ とし、`Monoid.op` と `Monoid.e` の計算量を
それぞれ $C_{op}$、$C_e$ とする。

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

保持領域はmap本体が $O(M)$、列挙の戻り値が出力node数に比例する。
