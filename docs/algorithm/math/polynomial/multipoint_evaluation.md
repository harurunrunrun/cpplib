---
title: Multipoint Evaluation (多点評価) [multipoint_evaluation]
documentation_of: ../../../../src/algorithm/math/polynomial/multipoint_evaluation.hpp
---

積の木（product tree）と剰余の木を使い、1 個の多項式を多数の点でまとめて評価する。評価点は重複していてもよい。

## API

```cpp
#include "src/algorithm/math/polynomial/multipoint_evaluation.hpp"

template<int MOD>
std::vector<Modint<MOD>> math::multipoint_evaluation(
    math::Polynomial<MOD> polynomial,
    const std::vector<Modint<MOD>>& points
);
```

戻り値の $i$ 番目は `polynomial(points[i])` である。評価点が空なら空の列を返し、零多項式はすべての点で $0$ になる。

内部では葉に $(X-p_i)$ を置いた積の木を作り、根から各部分木の積で割った余りを伝播させる。各葉に残る定数が評価値になる。

## 時間計算量・空間計算量

係数の個数を $n$、評価点の個数を $m$、長さ $k$ の多項式の積の計算量を $M(k)=O(k\log k)$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `multipoint_evaluation` | $O(M(n+m)\log(m+1))$ | $O((n+m)\log(m+1))$ |

## 注意点

- 剰余の計算で現れる非零の係数が `Modint<MOD>` 上で逆元を持つ必要がある。
- NTT に必要な 2 の冪の変換長が $\mathrm{MOD} - 1$ を割り切る必要がある。

## 内部API

`math::polynomial_internal` の次の定義は積の木の共有実装であり、互換性は保証しない。

- `ProductTree<MOD>(points)`: 各部分木の $\prod(X-p_i)$ を構築する。
- `point_count()`, `leaf_count()`: 評価点の個数と、それを 2 の冪に切り上げた葉の個数を返す。
- `operator[](node)`: 節点 `node` が表す積の多項式への `const&` を返す。
- `evaluate_product_tree_node(...)`: 剰余の木を再帰的に評価する。
- `evaluate_with_product_tree(polynomial, tree)`: 構築済みの積の木を再利用して全点を評価する。

| 内部API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `ProductTree(points)` | $O(M(m)\log(m+1))$ | $O(m\log(m+1))$ |
| `point_count` / `leaf_count`、`operator[]` | $O(1)$ | $O(1)$ |
| `evaluate_with_product_tree`（再帰の補助関数を含む） | $O(M(n+m)\log(m+1))$ | $O((n+m)\log(m+1))$ |

`operator[]` は範囲検査を行わない。返す参照は積の木のオブジェクトが生存している間だけ有効である。
