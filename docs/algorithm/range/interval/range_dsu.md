---
title: Range DSU (区間DSU)
documentation_of: ../../../../src/algorithm/range/interval/range_dsu.hpp
---

点同士の併合に加えて、区間内のすべての点を 1 点に併合できる DSU。

## コンストラクタ

```cpp
RangeDSU dsu(n)
```

## 関数

```cpp
dsu.leader(x)
dsu.same(x, y)
dsu.unite(x, y)
dsu.unite_range(l, r, x)
dsu.size(x)
dsu.same_range(l, r)
```

`unite_range(l, r, x)` は半開区間 $[l, r)$ のすべての点を `x` と同じ連結成分にする。

## 時間計算量

$I$ を現在の区間数、$K$ を対象範囲と交差して走査・削除する区間数とする。

| API | 時間計算量 |
| --- | --- |
| `RangeDSU(n)` | $O(n\log n)$（初期区間の `std::map` の構築） |
| 非 `const` `leader(x)` | 償却 $O(\alpha(n))$ |
| `const` `leader(x)` | $O(\log n)$（union by size による木の高さ） |
| `same(x,y)` | $O(\log n)$（`const` オーバーロード） |
| `unite(x,y)` | 償却 $O(\alpha(n))$ |
| `unite_range(l,r,x)` | $O((K+1)(\log I+\alpha(n)))$ |
| `size(x)` | $O(\log n)$（`const` `leader` を使用） |
| `same_range(l,r)` | $O((K+1)\log n+\log I)$ |

構築後の保持領域は $O(n+I)=O(n)$。`unite_range` と `same_range` の $K$ は、その呼出しで実際に走査する区間数である。

## 注意点

- `RangeDSU(n)` は $0,\ldots,n-1$ を別成分で初期化する。`n < 0` なら `runtime_error` を送出する。
- 非 const `leader(x)` は経路圧縮し、const オーバーロードは変更せず代表を返す。
- `same(x,y)` は同一成分かを、`unite(x,y)` は新しく併合したかを返す。
- `size(x)` は `x` の連結成分の要素数を返す。
- `unite_range(l,r,x)` は $[l,r)$ の各点と `x` を同じ成分にする。空区間では何もしない。
- `same_range(l,r)` は区間内の全点が同じ成分かを返し、空区間では `true` を返す。
- 点は $0\le x<n$、区間は $0\le l\le r\le n$ でなければならない。違反時は `runtime_error` を送出する。
