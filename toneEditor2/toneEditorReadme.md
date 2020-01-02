#   YMF825ToneEditor2



使い方は見てのとおりスライダを変更してパラメータをリアルタイムに変更するだけです
発音中、演奏中でも変更可。

### 設定

仮想MIDIケーブルを使いブリッジするデバイスを作り
ymf825.propertiesファイルでブリッジする入力のMIDIデバイス名,COMポートの通信速度
ブリッジするMIDIポートを指定します。
プロパティファイルは実行パスに置いてください。


(MIDIブリッジin)  
	midiStreamDeviceName=Trans  
COMポート指定  
	midiComPort=COM9  
ボーレート指定  
	baudRate=2000000    
音色ディレクトリ指定  
workDir=.\\tonedata\\
## 機能
### メニュー
##### Toneset
 Load Tone Set:		音色セットの読み込み  
 Sage Tone Set:  　音色セットの保存  
 Load from Device:  デバイスからの音色セット読み込み  
 exit:              プログラムの終了  

##### Tone
Load Tone:      音色の読み込み  
Save Tone       音色の保存  
Load from Device:  デバイスからの音色読み込み  


##### Edit
	Copy(1,2)to(3,4)        オペレータ間copy[1->3],[2->4]
	Copy(1,2)to(3,4)Clear   オペレータ間copy[1->3],[2->4]後[1,2]をクリア
	Copy(1,2)to(2,3)		オペレータ間copy[2->3],[1->2]
	Copy(1)to(2,3,4)		オペレータ間copy[1->2],[1->3],[1->4]
	View Sofware Modulation Pattern	  ソフトウェアモジュレーションビューワ表示
	Reset デバイスリセット
##### Mode
##### 発音モード
	mono:		モノモード、１Cｈで1音
	Poly:		ポリフォニックモード、１Chで再大16音発声(デフォルト）  
	D-Poly8:	2音同時発声ポリモード
				＋８した音色番号を＋８のMIDIchの発声として同時に鳴らす

![ToneEditor2-mesadd](https://user-images.githubusercontent.com/28349102/71668323-c2dad400-2dab-11ea-8124-949782d46150.png)
#### Envelope Viewer
エンベロープビューワの表示トグル

#### プリセットロード

プルダウンリストからGM準拠の音色？がロード出来ます。

#### チャンネルセレクト
YMF825の音色チャンネルの選択

#### 	Change Pair Tone
D-Poly8モード(２音同時発生）のﾍﾟｱ音色データに音色番号を変更

#### アルゴリズム選択
プルダウンでアルゴリズムを選択

#### copy and Swap Memory
音色メモリーへカレント音色をcopy  
音色メモリーとカレント音色をswap

#### EEPROMエリア
Inc,Decボタンの間のテキストエリアに数値が表示時使用可能に

	Inc:		音色ナンバーを＋１する
	Dec:		音色ナンバーを－１する
	EEPROM Save:	音色ナンバーのEEPROMエリアへカレント音色を保存
	EEPROM Read:	音色ナンバーのEEPROMエリアからカレント音色へ読み込み

#### テキストエリア音色READ,WRITE

	Write to text: 下部テキストエリアへカレント音色を出力(１０進ベタcsv、YAMAHA SysEX,DominoSysEX形式で)表示
							ボタンで選択。

	Read from Text: 下部テキストエリアからカレント音色から読み込み（ベタデータのみ）

### Envelope Viewer
![EnvelopeEditor-mesadd](https://user-images.githubusercontent.com/28349102/71668331-c9694b80-2dab-11ea-954f-afbb9e8b3c92.png)
###### Trace  
トレースモードトグル  
リリース位置やトレースポジション、オペレータを選択して特定の位置での音色を再現します  

###### PR (Position of Release)  
リリースタイミングの変更
###### OP1～OP4
トレースモード時のオペレータのON,OFF
###### Pos(Trace Position)
トレース時の発音タイミング
###### X3
横軸３倍表示  
###### note
トレース時の音の高さ変更

### Software Modulation Viewer
![SoftwareModulation-mesadd](https://user-images.githubusercontent.com/28349102/71668335-cd956900-2dab-11ea-9152-82dd83db3eca.png)
##### Modulation  
モジュレーションの強さ
##### Pitch
モジュレーション波形の変調倍率,
FM音源で言うところのキャリアのMultiples
##### Depth  
モジュレーション波形の変調強度
FM音源で言うところのキャリアのTotalLevel
##### Re Send
シーケンサ等でモジュレーションの値が再設定されたとき用に現在の値を再送信
##### CH1～CH16  
プルダウンメニューからMIDIチャンネルの選択
注：選択されるのはMIDIチャンネルでYMF825の音色チャンネルではありません！！
##### 基本波形
モジュレーションの基本波形選択
##### Delay
モジュレーションの開始時間
##### Rate  
モジュレーションの周波数



### 音色保存等
	フォーマットはバイナリのベタデータです
	Tone 30byte
	ToneSet 480byte










後述
元はOPL2用にシリアル通信で作ったプログラムだったのを
OPL3用にHID－MIDIの複合デバイスにして改造し、YM2151とDual用に書き換え
それをまたまたYMF825用にMIDIコマンドだけで制御するように作り変えたので
ソースがグダグダでとても恥ずかしくて公開出来ません。
waveイメージなんかフリーハンドで書いたもんですから
いつか綺麗に書き直したら公開するかも。
