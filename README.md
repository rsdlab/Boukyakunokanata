# 忘却の彼方
## 製品について
本製品は一日の会話を録音しすデバイスとなります。また、地図上に会話位置と音声をプロットします。\
ただし、このデバイスはGPS機能を用いているので基本的に屋外のみで使用可能です。

## ビルド
1.SPRESENSEのArduinoプロジェクトがビルドできる状態にします(詳細はSonyのSPRESENSE公式ページをご覧ください)。

2.ArudinoIDEでspresenseフォルダにあるirduino/irduino.inoを開いてください。

3.SPRESENSEをPCに接続します。

4.irduino.inoをMAINCORE上でスケッチをマイコンに書き込みます。

## 使い方
スケッチの書き込み、spreseseに電源を供給してください。

GPSによる座標取得が行われるまで30秒程度かかりますので待機してください。

忘却の彼方を首にかけて下さい。

## 機能
本製品の機能は以下の通りです。

・SPRESENSEのマイクから音声取得

・SPRESENSEのカメラにより画像取得

・SPRESENSEの高い精度のGNSSを用いた座標取得

・プログラムを実行して地図上に会話位置、内容を表示

## 会話位置、内容の表示（SDカードの差し込み口があるPCが必要）
### docker環境を利用する場合（ubuntu, windows環境のwlsのみ対応）\
以下のサイトからdockerをインストールしてください\
https://www.docker.com/

・foliumフォルダにあるdocker/build-docker.shを実行してください

・foliumフォルダにあるdocker/run-docker.shを実行してください

### docker環境を利用しない場合
・PC環境にanacondaなどをインストールしてpythonが実行できるようにしてください。\
anacondaのインストール：https://www.anaconda.com/

・foliumフォルダにあるrequirements.txtに記述されたライブラリをpython実行環境にインストールしてください

### 地図の生成
・SPRESENSEのSDカードに保存された以下のデータ\
「info.txt」,「jpg」, 「wav」\
をfoliumフォルダshared_dir/data内にコピーしてください

・foliumフォルダにあるshared_dir/folium_test.pyをpythonコマンドで実行してください

・foliumフォルダにあるshared_dir/beyond_oblivion.htmlをwifi環境で開いてください

・地図が表示され、位置を選択することで、時刻、画像、音声を確認することができます

・地図上に、赤マーカと青マーカが表示されます。画像上に人が映っていたとニューラルネットワークが判別したら赤マーカとなり、写っていなかったら青マーカとなります。