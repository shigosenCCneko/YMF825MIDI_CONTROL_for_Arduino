# YMF825をArduino UnoでMIDIデバイスとしてコントロールする

Arduino Uno を使いYMF825をMIDIデバイスとしてコントロールしてみました。  
USBシリアルMIDIデバイスとして振る舞うので、付属のToneEditorがMIDIシリアルブリッジとして働きます  
LoopMIDI等の仮想MIDIポートを利用して接続してください。
![-24bit -10-2](https://user-images.githubusercontent.com/28349102/40617916-c41bf018-62ca-11e8-8e40-fd6e0bc3234d.jpg)
接続速度は2,000,000Baud Rateになっています、別にシリアルMIDIブリッジを通して動かす場合は  
MidiCommand.cのSetupHardware()内のUBRR0の値を変更してください。  

ブリッジで使用するMIDIポート, COMポートはプロパティファイル"ymf825.properties"で指定します


###### BankSelect=1、にするとGM音源として振る舞いますがCh10は普通にメロディとして鳴ります。

### コンパイル
    YMF825board_sample4以下のファイルをArduinoIDEに読み込んでください  
    

### 接続


    本家YMF825サイトの接続例そのままにArduinoとYMF825boardを接続。
  	 RST_N- Pin9   
   	 SS   - Pin10
   	 MOSI - Pin11
   	 MISO - Pin12
   	 SCK  - Pin13

#### シリアルMIDデバイス
    USB-シリアルMIDIデバイスとしてArduinoのCOMポートへMIDIストリームを送ります。
	

## 音源部MIDIサポート

##### バンクセレクト、プログラムチェンジ　　
    BankSelect(cc0)を使ってプログラムチェンジでロードする音色データを選択（全MIDIチャンネルが変わります）

    0: YMF825の音色番号(No.1-No.16）   
    1: MA-3データで作ったGM音色(No.1-No.128)   
    2: EEPROM EEPROMからの音色読み出し(No.1-No.32)  

#### ピッチベンド、ピッチベンドセンシティブ
    ピッチベンドセンシティブ値0-24

#### システムエクルシーブメッセージ
     YMF825用に定義されたシステムエクルシーブメッセージを使って音色番号1-16を設定できます。

#### その他
    Velocity
    Hold
    Expression
    Modulation
    PartLevel
    
## コントロール部MIDIコマンド
音原設定等のコマンドやデータをシステムエクルシーブメッセージを利用して送信しています　　
規格に定められたヘッダ等は含んでいませんので他の機器にこのデータが流れないようにして下さい。

#### COMMANDフォーマット
    4バイトのデータをMSBを分けた7byteのメッセージ 
    フォーマット 
    "0xf0, COMMAND, Data1_bit6-0, Data2_bit6-0, Data3_bit6-0,MSB_Data1-3,0xf7"

	MSB_1-3:  0,0,0,0,0,MSB_Data1,MSB_Data2,MSB_data3
	
#### COMMAND
    0. YMF825のリセット
    1. レジスタへ書き込み DATA2=addres,DATA3=data
    2.Playモード設定
      DATA1=0 mono_mode
      DATA1=1 Poly_mode
      DATA1=3 Poly_mode2: 指定チャンネルの＋８チャンネルを同時発音させます
    5. EEPROM　write      Data1番の音色 Data2番地へData3を書き込み
    6. EEPROM tone read   EEPROMのDATA1番音色データを30byteのバイナリデータで返す
    9. write burst          音色バッファのYMF825へ全書き込み
    10. write data and burst 音色バッファData1番音色のDATA2番地へData3を書き込み後YMF825boardへ送信
			  	   	   
    11. write data only      音色バッファData1番音色Data2番地へData3を書き込み
    

