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

## ソフトウェアモジュレーション
  内部の波形テーブル(sin,三角波、鋸波)を使いFM音源の2オペレータの変調と同じ原理でモジュレーションの変換波形を作成します  
  MIDIのコントコールチェンジコマンド（CC)へ以下の機能を割り当てました

##### CC76 Vibrato Rate  
ビブラートの周期(0～127)
##### CC31 MoulationSinPitch
生成波形の変化周期(0～31)  
大きくすると変化パターンが細かくなります
(FM音源の２OPのキャリアのMultipleに相当)
##### CC32 ModulationSinDepth
生成波形の変化の大きさ(0～31)  
大きくすると変化パターンが複雑になります
(FM音源のキャリアのTotalLevel に相当)
##### CC59 SoftwareModulation  
モジュレーションの変換の強さ(0～31)
##### CC62 Mudulation Wave Select
ソフトウェアモジュレーションに使用する基本波形  
  0:  Sin Wave  
  1:  Triangle Wave  
  2:  Saw Wave  
##### CC63 Modulation Delay
モジュレーションの開始タイミング(0～127)  

## オペレータ操作コントロールコマンド
FM音源の各オペレータのTotal Level,Detune,Release Rateに対するコントロールを
以下のMIDIのコントコールチェンジコマンド（CC)へ割り当てました
#### Total PartLevel  (0～63)  
##### CC21  OP1 Total Level  
##### CC22  OP2 Total Level  
##### CC23  OP3 Total Level  
(OP4は指定不可)

#### Detune  (0～7)
##### CC52 OP1 Detune  
##### CC53 OP2 Detune  
##### CC54 OP3 Detune  
##### CC55 OP4 Detune

#### Release Rate (0～15)
##### CC112 OP1 Release Rate  
##### CC113 OP2 Release Rate  
##### CC114 OP3 Release Rate  
##### CC115 OP4 Release Rate  

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

## ToneEditor
toneEditor2ディレクトリにToneEditor.zipの形式で、javaの配布用JREを付けた実行可能jarをexe形式に変更したToneEditorArduino.exeを添付しています  
適宜の場所に解凍して実行してください。  
使用方法はtoneEditor2以下toneEditorReadme.mdをご覧になって下さい。　　
#### おまけ
ToneEditor/tonedata以下に不完全ですがDomino用の音源定義ファイル(YMF825midi.xml)と音色データ等を同梱しました。
