---
title: Static 2D KD Tree (静的二次元KD木) [DSL_2_C]
documentation_of: ../../../src/structure/spatial/kd_tree_2d.hpp
---

静的な二次元点集合に対して、軸平行な長方形の範囲検索を行う。
同じ点集合に対するユークリッド距離の最近傍探索にも対応する。

## 点の型

~~~cpp
KdTreePoint2D<T>{x, y, id}
~~~

`id` のデフォルト値は `-1` である。

## 構築

~~~cpp
KdTree2D<T, MAX_POINTS>()
KdTree2D<T, MAX_POINTS>(const vector<Point>& points)
void build(const vector<Point>& points)
~~~

深さごとに分割軸を交互に選び、中央値で分割する。各部分木はバウンディングボックスを保持する。
コピーは禁止されており、ムーブのみ可能である。

## 操作

~~~cpp
int size() const
bool empty() const
void clear()
vector<int> range_search(min_x, max_x, min_y, max_y) const
KdTreeNearestResult2D nearest_neighbor(x, y) const
~~~

`range_search` は閉長方形 `[min_x, max_x] × [min_y, max_y]` に含まれる点の `id` を列挙する。戻り値の順序は規定しない。

`nearest_neighbor` はユークリッド距離が最小の点を返す。距離が等しければ `id` が最小の点を選ぶ。

~~~cpp
struct KdTreeNearestResult2D {
    int id;
    long double squared_distance;
    bool found;
    bool exists() const;
};
~~~

点集合が空のとき、`found` と `exists()` は `false` になる。空でなければ、`id` は選ばれた点の `id`、`squared_distance` は距離の二乗である。`id` が `-1` の点も格納できるため、結果の有無は `exists()` で判定する。

## 時間計算量

$N$ を点数、$K$ を列挙する点数とする。座標の比較を $O(1)$ とする。

- デフォルトコンストラクタ: 固定長領域の初期化に $O(\texttt{MAX_POINTS})$
- `std::vector` を受け取るコンストラクタ: 固定長領域の初期化と `build` を含めて期待 $O(\texttt{MAX_POINTS}+N\log(N+1))$
- `build`: `nth_element` の平均計算量に基づき期待 $O(N\log(N+1))$
- `size`, `empty`, `clear`: $O(1)$
- `range_search`: 最悪 $O(N+K)$（戻り値の構築は $O(K)$）
- `nearest_neighbor`: 最悪 $O(N)$。均衡した KD 木と一般的な点分布では平均 $O(\log(N+1))$ が目安である。結果のメンバ参照と `exists()` は $O(1)$

均衡した KD 木の二次元直交範囲検索は、一般的な分布では $O(\sqrt{N}+K)$ が目安になるが、最悪計算量の保証は線形である。

## 空間計算量

- 固定長領域: $O(\texttt{MAX_POINTS})$
- 構築時の再帰スタック: $O(\log(N+1))$
- `range_search`: 再帰スタックと戻り値を含めて $O(\log(N+1)+K)$
- `nearest_neighbor`: 均衡時は再帰スタック $O(\log(N+1))$、最悪 $O(N)$

## 可変長版

```cpp
using Dynamic = KdTree2D<long long>;
```

容量のテンプレート引数を省略すると、実行時の点数に合わせて `std::vector` を確保する。構築は平均 $O(N \log N)$、保存領域は $O(N)$ である。範囲検索・最近傍探索の時間計算量は固定長版と同じで、最悪 $O(N)$ である。

容量を明示した固定長版も同じヘッダに含まれる。上記の固定上限に関する説明は、容量を明示した場合に適用される。

## 注意点

- 座標型は `long double` へ変換できなければならず、格納する点の座標は有限値であること。`nearest_neighbor` のクエリ座標が非有限値なら `invalid_argument` を送出する。
- 最近傍の距離の二乗は `long double` で計算する。極端に大きい座標や非常に近い浮動小数点座標では丸め誤差を含み得る。
- 固定長版で点数が `MAX_POINTS` を超える構築・`build` は `runtime_error` を送出する。
- `range_search(min_x, max_x, min_y, max_y)` は各下限が上限以下であること。逆転していれば `runtime_error` を送出する。
- 戻り値は点の `id` であり、`id` の一意性や範囲は構造側では検査しない。
- `clear` は $O(1)$ で論理的に空にし、次の `build` で領域を再利用する。
