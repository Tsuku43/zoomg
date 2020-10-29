# Algorithm

はじめに理想的な動画を用いての説明を行い,その後に実際の動画に適用する.理想的な動画とはバーチャル背景が赤色,オブジェクトが黄色,部屋が水色である動画とする.ここではオブジェクトが右から左へ横切ることを想定している.  
![sample_01.gif](https://github.com/Tsuku43/zoomg/blob/master/images/sample_01.gif?raw=true)  
※フレーム数確保のため速度を落としている.

## 手法
基本的な考え方としては,バーチャル背景が部屋を即座に隠すことを利用し,各ピクセルごとに出現時間が短い色を選ぶことで部屋を復元する.以下の画像は理想的な動画の同一部分を拡大し並べたものである.  
![sample_01_1-10.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_01_1-10.png?raw=true)  
バーチャル背景とオブジェクトの色には部屋の色よりも幅があり長時間表示されているため,出現頻度が高くなりやすい.よって,最も出現頻度が低い水色が,部屋の色だと決定できる.実際にはピクセルごとに処理を行うため,ある程度のフレーム数が必要である.以下の画像は理想的な動画の座標500,500の色を時間軸を横にとり,並べたものである.  
![sample_01_500_500_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_01_500_500_check.png?raw=true)  
水色が最も少ないことがわかる.フレーム数と動画長が増加するほど復元の精度が向上する.問題はバーチャル背景と部屋の色が境界で混ざり合う場合である.これはバーチャル背景と部屋の色差を利用して,排除している.バーチャル背景は先に説明した部屋の復元の逆,最も出現頻度が高い色を選択することで取得できる.今後,色差計算が精度向上に大きく影響する可能性がある.

### 理想的な動画
理想的な動画および,部屋部分を写真に変更した理想的な動画にzoomgを適用する.復元された画像は以下になる.  
**理想的な動画**  
部屋部分  
![sample_01_room.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_01_room.png?raw=true)  
500,500の時間変化  
![sample_01_500_500_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_01_500_500_check.png?raw=true)  
**部屋部分を写真に変更した理想的な動画**  
部屋部分  
![sample_02_room.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_02_room.png?raw=true)  
500,500の時間変化  
![sample_02_500_500_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_02_500_500_check.png?raw=true)  
共にパラメータを20とした.フレーム数の関係でバーチャル背景のみとなったピクセルは縦の線として現れている.オブジェクトと同色のクラスタはパラメータ調整により削減できる.パラメータを5としたsample_02の部屋部分を以下に示す.  
![sample_02_room_5.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_02_room_5.png?raw=true)

### 実際の動画
実際のzoomから得られた動画にzoomgを適用する.復元された画像は以下になる.  
**復元された画像**  
部屋部分(顔黒塗り)  
![zoom_0_room.png](https://github.com/Tsuku43/zoomg/blob/master/images/zoom_0_room.png?raw=true)  
500,500の時間変化  
![zoom_0_500_500_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/zoom_0_500_500_check.png?raw=true)  
パラメータを90とした.フレーム数の関係で腕などの表示時間が短いため,背景と判定されている.大幅にバーチャル背景が乱れる現象も復元には大きく影響している.実際の部屋と比べると,赤く囲った部分が復元できていることがわかる.  
**実際の部屋**  
![room.png](https://github.com/Tsuku43/zoomg/blob/master/images/room.png?raw=true)  

## 対抗手法
グリーンバックを使えば部屋の情報が漏れる心配はない.技術的な対策としては,バーチャル背景を不規則に動かすことが挙げられる.規則的に動く場合,バーチャル背景のピクセルが累積してしまうので動画時間によっては復元される恐れがある.バーチャル背景の画像を十分長さのある動画とし,色と場所をランダムにしたノイズピクセルをばらまくことで機械的な復元を妨げることができる.  

### フレーム単位のノイズ
理想的な動画をフレーム単位で分割し,1つのフレームのバーチャル背景部分をノイズに変更したものを再度動画にしたものにzoomgを適用する.  
![image_123.png](https://github.com/Tsuku43/zoomg/blob/master/images/image_123.png?raw=true)  
フレーム単位で挿入するため,オブジェクトが動いた後の動画後半に挿入した.これは得られた部屋の情報がノイズによってかき消されることを意味している.復元された画像は以下になる.
**復元された画像**  
部屋部分  
![sample_03_room.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_03_room.png?raw=true)  
500,500の時間変化  
![sample_03_500_500_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_03_500_500_check.png?raw=true)  
1000,1000の時間変化  
![sample_03_1000_1000_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/sample_03_1000_1000_check.png?raw=true)  
パラメータを20とした.ノイズによって復元が妨げられていることがわかる.  

### ピクセル単位のノイズ
フレーム単位のノイズでは,フレームを削除することにより容易に復元が可能となる.これを防ぐため,バーチャル背景のランダムな箇所へピクセル単位でのノイズを挿入する.ノイズのピクセルサイズをもランダムにすることで予測を困難にすることができるが,動画の質が低下してしまう.本対策ではバーチャル背景の認識に影響を与えない程度のノイズを目指し,ノイズの座標と量をランダムに散りばめることとした.ノイズを含ませたバーチャル背景用動画をzoomのバーチャル背景として使用した動画にzoomgを適用する.動画は![zoom_vbg.png](https://github.com/Tsuku43/zoomg/blob/master/images/zoom_vbg.png?raw=true)より生成したもので,読み込みの関係でmovへの変換を行った.  
**復元された画像**  
部屋部分(顔黒塗り)  
![zoom_1_room_60.png](https://github.com/Tsuku43/zoomg/blob/master/images/zoom_1_room_60.png?raw=true)  
![zoom_1_room_90.png](https://github.com/Tsuku43/zoomg/blob/master/images/zoom_1_room_90.png?raw=true)  
500,500の時間変化  
![zoom_1_500_500_check.png](https://github.com/Tsuku43/zoomg/blob/master/images/zoom_1_500_500_check.png?raw=true)  
上の画像をパラメータ60,下の画像をパラメータを90とした.ノイズにより部屋の復元がさらに困難になっていることがわかる.  

