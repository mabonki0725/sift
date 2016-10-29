##SIFT(Scale Invariant Feature Transform)特徴量の計算

_処理_  
1. P3 or P2 PPMファイルの読込み  
2. Gussian Filterの異なるσ幅での画像の平滑化  
3. 隣合うσ幅の平滑化画像の差を特徴点とする  
4. 特徴点の曲率が閾値以下なら削除（点列と判断）
5. 特徴点の極大値が閾値以上なら採用  
6. 特徴点間の強度と方向を計算、強度が閾値以上なら採用  

![計算結果]](https://cloud.githubusercontent.com/assets/20177544/19826297/8d463172-9dc1-11e6-96a8-ea699c152f5b.jpg)

_参照_ 
[SIFTを1から組む]http://d.hatena.ne.jp/colorcle/20090629/1246292723

