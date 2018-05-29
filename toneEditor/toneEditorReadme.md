#   YMF825ToneEditor
 


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
      
      
![toneeditor](https://user-images.githubusercontent.com/28349102/40658976-db803bde-6387-11e8-97c6-a3d419bd29db.jpg)
### プリセットロード	

上部のプルダウンリストからGM準拠の音色？がロード出来ます。


### 発音モード
発音モードを３種類設定可  

	モノモード  
		１チャンネル１音  

	ポリモード  
		最大16音同時発声、デフォルトはこれ  

	2音同時発声ポリモード  
		＋８した音色番号を＋８のMIDIchの発声として同時に鳴らします  
		擬似８オペレータになる訳ですけど  
		これキーボードで弾いて遊ぶと楽しいです。  
		


### 	GotoPair：
D-Poly8モード用にﾍﾟｱの音色データに音色番号を変更

### モードエリア
	mono:		モノモード、１Cｈで1音
	Poly:		ポリモード、１Chで再大16音発声
	D-Poly8:	2音同時発声ポリモード


### 音色保存等
	フォーマットはバイナリのベタデータです

	WaveSetSave:	１６音全部まとめてファイルへ保存
	WaveSetLoad:	１６音全部まとめてファイルから読み込み

	waveSave:	カレント音色をファイルへ保存
	waveload:	カレント音色へファイルから読み込み

	Write to text: 下部テキストエリアへカレント音色を(１０進ベタcsv、YAMAHA SysEX,DominoSysEX形式で)表示
							ボタンで選択。

	Read from Text: 下部テキストエリアからカレント音色へ読み込み

### EEPROMエリア
Inc,Decボタンの間のテキストエリアに数値を書きこんだら使用可能に

	Inc:		音色ナンバーを＋１する
	Dec:		音色ナンバーを－１する
	EEPROM Save:	音色ナンバーのEEPROMエリアへカレント音色を保存
	EEPROM Read:	音色ナンバーのEEPROMエリアからカレント音色へ読み込み


後述
元はOPL2用にシリアル通信で作ったプログラムだったのを
OPL3用にHID－MIDIの複合デバイスにして改造し、YM2151とDual用に書き換え
それをまたまたYMF825用にMIDIコマンドだけで制御するように作り変えたので
ソースがグダグダでとても恥ずかしくて公開出来ません。
waveイメージなんかフリーハンドで書いたもんですから
いつか綺麗に書き直したら公開するかも。

