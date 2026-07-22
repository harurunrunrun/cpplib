---
title: Micali-Vazirani General Matching (Micali-Vazirani法による一般グラフ最大マッチング)
documentation_of: ../src/algorithm/matching/general/micali_vazirani_matching.hpp
---

無向一般グラフの最大濃度マッチングを求める。二部グラフでなくてもよく、奇閉路を含むグラフを扱える。

## 公開型

### `MicaliVaziraniMatchingResult`

```cpp
struct MicaliVaziraniMatchingResult{
    int cardinality;
    std::vector<int> mate;
};
```

| メンバ | 説明 | 操作の時間計算量 |
|---|---|---|
| `cardinality` | 最大マッチングの辺数。 | 参照は $O(1)$ |
| `mate[v]` | 頂点 `v` とマッチした頂点。未マッチなら `-1`。長さは常に頂点数。 | 参照は $O(1)$ |

結果のコピーは `mate` をコピーするため $O(N)$、ムーブは $O(1)$。

## 構築

```cpp
explicit MicaliVaziraniMatching(int vertex_count)
```

頂点集合を `[0, vertex_count)` とする空グラフを構築する。

- 時間計算量: $O(1)$
- 追加領域: $O(1)$
- `vertex_count < 0` なら `std::runtime_error`

オブジェクトのコピー構築・コピー代入は追加済み辺をコピーするため $O(M)$。ムーブ構築・ムーブ代入は $O(1)$、破棄は $O(M)$。

## 頂点数

```cpp
int vertex_count() const noexcept
```

構築時の頂点数を返す。

- 時間計算量: $O(1)$
- 追加領域: $O(1)$
- 例外を送出しない

## 辺数

```cpp
std::size_t edge_count() const noexcept
```

保持している無向辺数を返す。有効な自己ループは無視されるため数えない。多重辺は追加された本数をそれぞれ数える。

- 時間計算量: $O(1)$
- 追加領域: $O(1)$
- 例外を送出しない

## 辺の追加

```cpp
void add_edge(int left, int right)
```

無向辺 `{left, right}` を追加する。自己ループは最大マッチングに寄与しないため無視する。多重辺を許す。

- 時間計算量: 償却 $O(1)$
- 追加領域: 償却 $O(1)$
- いずれかの端点が `[0, vertex_count)` の外なら `std::runtime_error`
- 範囲検査は自己ループ判定より先に行う

`solve()` の後にも辺を追加できる。次の `solve()` は追加後の全辺を使って最初から計算する。

## 最大マッチング

```cpp
MicaliVaziraniMatchingResult solve() const
```

最大濃度マッチングの辺数と対応関係を返す。オブジェクトを変更しないため、同じグラフに繰り返し呼び出せる。

- 時間計算量: $O(N + M\sqrt{N})$
- 追加領域: $O(N+M)$
- 長さ $N$ の `mate` の構築時間を含む
- メモリ確保に失敗した場合は標準ライブラリの確保例外を送出し得る

最大マッチングが複数ある場合、どれを返すかは辺の追加順に依存し得る。`cardinality` とマッチする辺集合だけが保証対象であり、特定の `mate` 配列を仮定してはならない。

## アルゴリズムと計算量の根拠

各phaseで次を行う。

1. 偶奇の最短交互路レベルをBFSで構築し、非prop辺をtenacityごとのbridge bucketへ入れる。
2. 同じtenacityのbridgeをDDFSで処理し、奇閉路をpetalへ縮約する。各petalの代表はbudで管理する。
3. 最短増加路を見つけたらpetalを開き、同じphase中で頂点素な最短増加路をすべて増加する。使用済み頂点はpredecessor数を更新しながら探索対象から除く。

1 phaseでは各辺を定数回だけ走査する。bud検索には、探索木に対する専用のincremental tree set unionを使う。大きさ
$B=\Theta(\log N/\log\log N)$ のmicrosetについて、親ポインタ表現とlink bit列からbudを引く表を前計算する。microset内の `grow`、`link`、`find` は表引きで $O(1)$、大きさ $B$ になったときの分割は $O(B)$ である。分割間には $\Omega(B)$ 回の `grow` があるため、全分割時間は $O(N)$ になる。

macro側で通常のunion-findを使う対象は、分割で生じた $R=O(N/B)$ 個のmicroset rootだけである。実際のmacro操作列はphase内の $O(N+M)$ 操作列のprefixとみなし、末尾に定数時間のroot検索を補って長さ $Q=\Theta(N+M)$ にできる。孤立点を除いたcoreでは $M=\Omega(N)$ なので $Q/R=\Omega(B)$。union-by-sizeとpath compressionの二引数bound $O(Q\alpha(Q,R))$ では、この比が $\Omega(\log N/\log\log N)$ のとき $\alpha(Q,R)=O(1)$ である。実際に実行するprefixの時間は補った列以下なのでmacro側も $O(N+M)$。したがって通常の一般DSUを全頂点へ直接使ったときの逆Ackermann因子は残らず、1 phaseは $O(N+M)$。

孤立点は探索coreへ入れない。辺があるcoreでは頂点数が $O(M)$ なので、各phaseは $O(M)$ と書ける。Micali-Vaziraniのphase boundによりphase数は $O(\sqrt N)$ であり、最後に全頂点の `mate` を生成する $O(N)$ を足して全体は $O(N+M\sqrt N)$。

microsetの全状態表はcore頂点数以下（小入力では固定定数以下）になるblock sizeを選ぶため、前計算の時間・空間も $O(N)$。実装は再帰的な交互路展開を明示stackへ置き換えており、グラフの深さに比例するC++呼出しstackを使わない。

## 使用例

```cpp
MicaliVaziraniMatching graph(5);
graph.add_edge(0, 1);
graph.add_edge(1, 2);
graph.add_edge(2, 0);
graph.add_edge(3, 4);

const auto result = graph.solve();
// result.cardinality == 2
for(int vertex = 0; vertex < graph.vertex_count(); vertex++){
    if(result.mate[vertex] != -1 && vertex < result.mate[vertex]){
        // vertex -- result.mate[vertex] is a matching edge
    }
}
```

## 時間計算量

$N$ を頂点数、$M$ を自己ループを除いて追加された辺数とする。

| API | 時間計算量 |
|---|---|
| constructor | $O(1)$ |
| copy constructor / copy assignment | $O(M)$ |
| move constructor / move assignment | $O(1)$ |
| destructor | $O(M)$ |
| `vertex_count()` | $O(1)$ |
| `edge_count()` | $O(1)$ |
| `add_edge(left, right)` | 償却 $O(1)$ |
| `solve()` | $O(N+M\sqrt N)$ |
| 結果のメンバ参照 | $O(1)$ |
| 結果のコピー | $O(N)$ |
| 結果のムーブ | $O(1)$ |

## 空間計算量

グラフ本体は $O(M)$、`solve()` の作業領域と戻り値は $O(N+M)$。

## 注意点

- 無向・重みなしの最大濃度マッチングである。最大重みマッチングや二部グラフ専用APIではない。
- 頂点番号は0-indexed。
- 自己ループは無視し、多重辺を許す。多重辺が答えの濃度を変えることはない。
- `mate[mate[v]] == v` が成立し、未マッチ頂点だけが `-1` を持つ。
- 計算量は、$\Omega(\log N)$ bitの整数・ポインタ操作を $O(1)$ と数えるunit-cost RAMを仮定する。
- 頂点数と辺数、および内部配列長が `int` と `std::size_t` の表現範囲に収まり、必要なメモリを確保できることを前提とする。
