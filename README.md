# あらまし

Twitter ではやっている [ちょまど問題][chomado] ですが、  
誰かが全探索に基づく方法を試さないといけないと思い実装しました。  
余裕で指数時間かかるので辛いです。

[chomado]: http://togetter.com/li/682030 "何回で満点とれる？【ちょまど問題に挑む人々】 - Togetterまとめ"

## (一般化) ちょまど問題

m 択問題が n 問与えられたとする。  
解答者は必ず全問に 1 ~ m の解答を与えた上で採点者に自身の解答を提出する。  
採点者は提出された解答中の正答数のみを解答者に返す。  
解答者は「『最悪の提出回数』を最小化する解答戦略」を求めたい。  


## 方針

全探索といっても何を探索するのか…。  
『解答戦略』を全探索するのは辛いので、「全探索に基づく解答戦略」を作ります。  
そして何を探索するかですが、「現時点で最も情報量が多くなる解答」を全探索します！  
以下で詳しく説明します…。  

### Notation

まず以下とする。

    `A = \{1,...,m\}^n` : すべての解答の集合
    `V = \{0,...,n\}`   : すべての評価値の集合
    `y \in A`           : 観測できない真の解答

我々は任意の解答 `x \in A` に対して `hamming(x, y)` を評価することができる。  
ここで `hamming(x, y)` は `x` と `y` のハミング距離であり以下のように定義される。  

    `hamming(x, y) = |\{i \in \{1,...,n\} | x_i != y_i\}`

つまり正答数は `n - hamming(x, y)` である。  
ここでは簡単のため、`hamming(x, y)` の値を `x` の評価値と呼び、単純に `h(x)` と書く。  
更に以下とする。  

    `x_i \in A` : `i` 回目に提出した解答  
    `h_i \in V` : `x_i` の評価値 (`= h(x_i)`)
    `x_{1,i} = (x_1,...,x_i)` : 解答列  
    `h_{1,i} = (h_1,...,h_i)` : x_{1,i} に対応する評価列  

更に以下の候補集合を導入する。  

    `C_0 = A`  
    `C_i = \{c \in C_{i-1} | hamming(c, x_i) = h_i\}`  

`C_i` は解答列 `x_{1,i}` を提出して評価列 `h_{1,i}` を得ることが出来るすべての `A` 中の要素である。  
`y` は必ず `C_i` に含まれ、 `|C_i| = 1` ならば `C_i = \{y\}` である。  

解答 `x_i` は以下の解答戦略 `S` より関数的に決定する。

    `S(x_{1,i}, h_{1,h}) = x_{i+1}`

ある真の解答 `y` と解答戦略 `S` が与えられたとき、  
`h_i = 0` または `|C_i| = 0` となる最小の `i` を _必要解答数_ と呼び、`s(y, S)` と書く。  
更に `s(S) = max_{y \in A} s(y, S)` を `S` の _最悪必要解答数_ と呼ぶ。  
`s(S) < c` なる整数 `c` が存在する `S` のことを _停止性_ を持つ戦略と呼ぶことにする。

我々の最終ゴールは最悪必要解答数 `s(S)` を最小化する `S_opt` と
その時の最悪必要解答数 `s(S_opt)` を求めることである。



#### [余談] 愚直な戦略

今、`A` 上に順序を導入し、`succ(x)` を `x` の次の要素とする。  
この時、以下の解答戦略 `S` は停止性を持つ。  

    `S(x_{1,i}, h_{1,i}) = succ(x_i)`

この解答戦略 `S` は `A` 中の要素を順番に評価するだけである。  
よって `s(S) = |A|` である。  



### 情報量に基づく全探索による戦略

`x \in A`, `h \in V`, `C \subseteq A` に対して `N(x, h, C)` を以下とする。  

    `N(x, h, C) = |\{c \in C | hamming(x, c) = h\}|

つまり `N(x, h, C)` は `C` 中の `hamming(x, c) = h` となる `c` の数である。  

更に `x`, `C` に対して `I(x, C)` を以下とする。  

    I(x, C) = - \sum_{h \in V} p(x, h, C) log p(x, h, C)
    p(x, h, C) = N(x, h, C) / N(x, C)
    N(x, C) = \sum_{h \in V} N(x, h, C)

つまり `I(x, C)` は解答 `x` を提出した時に得られる平均情報量である。  

この情報量を元に解答戦略 `S` を以下のように定める。

    `S(x_{1,i}, h_{1,i}) = argmax_{x \myin A} I(x, C_i)`

つまりこの解答戦略 `S` は現時点の解候補 `C_i` を考えたとき、  
得られる平均情報量を最大化するように次の戦略 `x_{i+1}` を決定する。  

ここで `argmax_{x \myin A} I(x, C_i)` を直接求めることができるか、という問題があります。  
自分は 10 分考えても分からなかったので全探索することにしました。  
そしてここにあるのがそのソースです。  


### 探索結果

プログラムを実行したところ以下の結果を得ました。

    N =  1 : |A| =       4, W = 3, T = 3.252029e-04
    N =  2 : |A| =      16, W = 4, T = 1.171112e-03
    N =  3 : |A| =      64, W = 5, T = 4.436016e-03
    N =  4 : |A| =     256, W = 6, T = 1.532793e-02
    N =  5 : |A| =    1024, W = 7, T = 7.411599e-02
    N =  6 : |A| =    4096, W = 8, T = 9.646468e-01
    N =  7 : |A| =   16384, W = 8, T = 1.568699e+01
    N =  8 : |A| =   65536, W = 9, T = 3.105097e+02
    N =  9 : |A| =  262144, W = 9, T = 4.426311e+03
    N = 10 : |A| = 1048576, W = 10, T = 7.492757e+04

実行時間が指数時間に増えてますね…。  
どうやら１０回はかかってしまうようです。  
今回は平均情報量を最大化する貪欲法でしたが、  
次回は最悪事故情報量を最大化する貪欲法で試してみようかな。
