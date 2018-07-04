incl 文件夹中含所用部分头文件
SoundStretch 文件夹为正式工程文件
SoundTouch 为附属工程文件
data 文件中是示例数据


使用方法：
打开SoundStretch文件，VC2017打开工程文件。点击Debug运行。

在命令行模式中使用命令：


soundtouch + input_filename + output_filename + [swtich]

如下为命令选项：
    "  -tempo=n : Change sound tempo by n percents  (n=-95..+5000 %)\n"
    "  -pitch=n : Change sound pitch by n semitones (n=-60..+60 semitones)\n"
    "  -rate=n  : Change sound rate by n percents   (n=-95..+5000 %)\n"
    "  -bpm=n   : Detect the BPM rate of sound and adjust tempo to meet 'n' BPMs.\n"
    "             If '=n' is omitted, just detects the BPM rate.\n"
    "  -quick   : Use quicker tempo change algorithm (gain speed, lose quality)\n"
    "  -naa     : Don't use anti-alias filtering (gain speed, lose quality)\n"
    "  -speech  : Tune algorithm for speech processing (default is for music)\n"
    "  -license : Display the program license text (LGPL)\n";



命令示例：

soundtouch ../data/qw.wav ../data/1.wav -rate=-10


注意：
1、soundtouch 不能写错，区分大小写
2、输入音频暂时只能 .wav 格式的
