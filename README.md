oplen
=====

x86とx64のマシン語サイズを求めるライブラリ

x86とx64の マシン語バイトを食べさせると、長さを求めるライブラリです。
sexyhookのサブプロダクトとして作りました。

一応、そこそこ動いているみたいです。
たまに、うまくいかないで失敗して死にます。



遊び方
===

const SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE cputype = SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE_X86;
//or const SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE cputype = SEXYHOOKFuncBase::SEXYHOOK_CPU_ARCHITECTURE_X64;
SEXYHOOKFuncBase::OperandLength(const unsigned char* code,cputype); でマシン語コードが何バイトか返します。



応用方法
===

ライブパッチとか効率的なトランポリン製造とか、そこらへん。
ライセンスが非常に緩いので多少応用がある？


ライセンス
===
ライセンスは NYSLとか GPLとか LGPLとか BSDとか、MITとか 貴方が好きなライセンスで勝手に使ってください。

