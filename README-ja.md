# PD-Camera for M5Stack

[English](./README.md)

このプロジェクトは [PD-Camera project](https://github.com/t0mg/pd-camera) のハードウェア部分を [M5Stack CoreS3](http://docs.m5stack.com/en/core/CoreS3) に置き換えて動作するようにしたものです。

![pd-camera-m5](pd-camera-m5.jpg)

>PD-Camera is an unofficial, experimental accessory for [Panic Inc.](https://panic.com/)'s [Playdate](https://play.date) handheld console, which as the name hopefully suggests allows the device to record pictures as well as animated gifs, in whopping  1-bit color and QVGA (320x240) resolution. Kind of similar to the [1998 accessory for that other monochrome handheld device](https://en.wikipedia.org/wiki/Game_Boy_Camera) (which has 2-bit color and fewer pixels).
>
>-- <cite>[PD-Camera project](https://github.com/t0mg/pd-camera/tree/main#pd-camera-project)</cite>


詳細は、[PD-Camera project](https://github.com/t0mg/pd-camera) をご覧ください。

## PD-Camera for M5Stack の特徴

- Playdate にコンパニオンアプリ、 M5Stack CoreS3 に PD-Camera for M5Stack ファームウェアをインストール。あとは USB ケーブルで接続するだけ。驚くほど簡単に動作します！

- M5Stack CoreS3 には電源やカメラが初めから接続されているため、半田付けなどの手順は必要ありません。

- PD-Camera for M5Stack には専用のカバー、ケースは用意されていません。

## 免責事項

>This project is 
>- not affiliated with nor endorsed by [Panic Inc.](https://panic.com/), 
>- highly experimental,
>- very hacky,
>   - oh so hacky.
>
>While I crashed my own Playdate countless times working on this, and it has survived so far, I cannot be held responsible for any damage, data loss, bad selfie, pandemic, etc. that might occur to you while playing with this project. Use this at your own risk. Thanks ❤️.
>
>-- <cite>[PD-Camera project](https://github.com/t0mg/pd-camera/tree/main#disclaimer)</cite>

PD-Camera for M5Stack も同じくとても実験的です。自己責任で楽しんでください！

## ハードウェア

動作させるには以下のハードウェアが必要です。

- [Playdate](https://play.date)

- [M5Stack CoreS3](http://docs.m5stack.com/en/core/CoreS3)

- USB-C ケーブル

## コンパニオンアプリ

Playdate にサイドロードする必要があるコンパニオンアプリは、オリジナルプロジェクトの [PD-Camera project App](https://github.com/t0mg/pd-camera-app) をそのまま利用できます。

## ファームウェア

[M5Burner](https://docs.m5stack.com/en/download) から PD-Camera project for M5Stack ファームウェアを変更する必要がない場合は、そちらをご利用ください。

## ビルド

このファームウェアは [PlatformIO IDE for VSCode](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)を使って開発しています。ビルドの詳しい手順は [PlatformIO IDE for VSCode](https://docs.platformio.org/en/stable/integration/ide/vscode.html) をお読みください。

## 既知の問題

- v0.0.1 時点では、動作が不安定です。USB 通信が途切れたり、コンパニオンアプリ、ファームウェアがフリーズ、クラッシュすることがあります。


- USB 通信のデータをまれに取りこぼしてしまうことがあるため、プログラムで回避している部分があります。

- コンパニオンアプリの[既知の問題](https://github.com/t0mg/pd-camera-app#known-issues) も、同様に再現することがあります。

## 依存しているライブラリ

- [PD-Camera project](https://github.com/t0mg/pd-camera) by [t0mg](https://github.com/t0mg)

    PD-Camera project のソースコードを元に、M5Stack で動作するファームウェアを作成しました。

- [Template Repository for Generating User Custom Firmware for M5Burner v3](https://github.com/3110/m5burner-user-custom-platformio-template) by [SAITO, Tetsuya](https://github.com/3110) 

    [generate_user_custom.py](https://github.com/3110/m5burner-user-custom-platformio-template/blob/main/generate_user_custom.py) で公開用のファームウェアを作成しました。

- [dithering_halftoning](https://github.com/deeptronix/dithering_halftoning) by [deeptronix](https://github.com/deeptronix)

- [M5Unified](https://github.com/m5stack/M5Unified) by [lovyan03](https://github.com/lovyan03) 

- [ESP32TinyUSB](https://github.com/chegewara/EspTinyUSB) by [chegewara](https://github.com/chegewara)

- [esp32-camera](https://github.com/espressif/esp32-camera) by [Espressif Systems](https://github.com/espressif)


## 参考にさせていただいたサイト

- [playdate-reverse-engineering](https://github.com/jaames/playdate-reverse-engineering) by [jaames](https://github.com/jaames)

- [M5CoreS3_CameraTest](https://github.com/ronron-gh/M5CoreS3_CameraTest) by [motoh](https://github.com/ronron-gh)

- [esp32-usb-host-demos](https://github.com/touchgadget/esp32-usb-host-demos) by [touchgadget](https://github.com/touchgadget)

- [EspUsbHost](https://github.com/tanakamasayuki/EspUsbHost) by [TANAKA Masayuki](https://github.com/tanakamasayuki)

- [ESP32-S3 USB Hostの調査1](https://note.com/ndenki/n/n2bba54a9b3cc) by [えぬでんき](https://note.com/ndenki)

## 謝辞

とても素晴らしい、ワクワクする [PD-Camera project](https://github.com/t0mg/pd-camera) をリリースしてくれた、[t0mg](https://github.com/t0mg)さんに、心より感謝しています！！

----

 Playdate は [Panic Inc.](https://panic.com/) の製品です。このプロジェクトは、いかなる形でも彼らと提携しているわけでも、彼らの支持を受けているわけでもありません。
 