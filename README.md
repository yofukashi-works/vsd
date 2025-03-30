# [VSD プロジェクト yofukashi-works Fork版](https://github.com/yofukashi-works/vsd)
======================

## yofukashi-works Fork版 について
本プロジェクトは元のプロジェクトである、[vsd](https://github.com/yoshinrt/vsd) がPublic archive化されたため  
[yofukashi-works](https://github.com/yofukashi-works)が作製した**Fork版**です。

微妙になんらかの更新を入れています。

### 変更履歴
* r1023
  * インストーラの修正
  * AviUtlを1.10に変更
  * 拡張編集0.92を追加
  * L-SMASH-Worksをr1151 Mr-Ojiiに変更
  * ffdshowをrev4532に変更
* r1022
  * 2^24を超える入力値で、丸め誤差が発生する問題を修正
  * 開発環境をVS2019→VS2022に変更
  * 開発環境をcygwin→cygwin64に変更
* r1021
  * オリジナル最終版

## ライセンス情報
vsdプロジェクトの著作権は元の作者である[yoshinrt](https://github.com/yoshinrt) に帰属し、本プロジェクトもGNU General Public License (GPL-2.0 license) に基づいて公開します。

## vsdに関する記事
よふかしわーくすの[ブログ](https://www.blog.yofukashi-works.com/?s=VSD)にて、いくつかVSD for GPSに関する記事を公開しています。

## マニュアル
[VSD for GPS yofukashi-works Fork版](https://yofukashi-works.github.io/vsd/)で公開しています。


## VSD プロジェクト オリジナル版
VSD プロジェクトは，自動車のサーキット走行などにおけるデータロガー環境を自作することを目指し，車両情報の取得，車両情報のリアルタイム表示，
オフラインでの車両情報と車載動画との合成，を実現するプロジェクトです．

以下の成果物を含んでいます．  

* `vsd_filter`: GPS ログ・データロガーのログを車載動画に合成する [VSD for GPS](https://yoshinrt.github.io/vsd/)
* `vsd2`: ARM マイコンボード (DesignWave 2008年5月号付属) を，車両データ取得用データロガーとするための Firmware
* `vsdroid`: 上記 ARM マイコンの UI として動作するための Android アプリ
* `scripts`: 開発過程で作成した多雑なスクリプト

以下は過去の産物であり，すでに未メンテ

* `deprecated/vsd`: 秋月 H8 3664 マイコンボード (すでに廃盤?) 用 Firmware
* `deprecated/seri2eth`: Fonera+ (FON 2201) を UART⇔Ether コンバータとして使用するためのプログラム
* `deprecated/vsd_psp`: PSP 用 LUA playre で動作する，UI アプリ
* `deprecated/vsdwin`: Windows 用 UI アプリ

正式リリース以外の最新バイナリは，[AppVeyor](https://ci.appveyor.com/project/YoshiNRT/vsd/branch/master/artifacts) で公開しています．

ビルド方法 (vsd_filter のみ)
------
### 必要な環境 ###
* Microsoft Visual Studio Community 2019
* cygwin (make, bash, perl, git, zip, tar, bzip2, etc...)
  * ビルド時に ScriptIF.h, rev_num.h がないと怒られるときは，cygwin の実行に失敗しています．[こちら](https://ci.appveyor.com/project/YoshiNRT/vsd/branch/master/artifacts)からダウンロードできます．

### ビルド手順 ###
1. vsd ソースコード一式を git clone --recursive で取得してください．
1. コンパイル済みの [google v8 ライブラリ](https://github.com/yoshinrt/vsd/releases/download/release_r986/v8_lib_ReleaseMT.tbz) をダウンロードし，vsd_filter\v8_lib 以下に展開してください．階層的には vsd_filter\v8_lib\ReleaseMT などのようになります． 
1. vsd_filter\vsd_filter.sln を W クリックし，Visual Studio を立ち上げます．
1. プロジェクト vsd_filter，ソリューション構成 ReleaseMT を選んでビルドすると，通常リリースする VSD for GPS のプラグインがビルドされます．
1. release.bat を実行すると，インストーラパッケージを生成します．
