incl �ļ����к����ò���ͷ�ļ�
SoundStretch �ļ���Ϊ��ʽ�����ļ�
SoundTouch Ϊ���������ļ�
data �ļ�����ʾ������


ʹ�÷�����
��SoundStretch�ļ���VC2017�򿪹����ļ������Debug���С�

��������ģʽ��ʹ�����


soundtouch + input_filename + output_filename + [swtich]

����Ϊ����ѡ�
    "  -tempo=n : Change sound tempo by n percents  (n=-95..+5000 %)\n"
    "  -pitch=n : Change sound pitch by n semitones (n=-60..+60 semitones)\n"
    "  -rate=n  : Change sound rate by n percents   (n=-95..+5000 %)\n"
    "  -bpm=n   : Detect the BPM rate of sound and adjust tempo to meet 'n' BPMs.\n"
    "             If '=n' is omitted, just detects the BPM rate.\n"
    "  -quick   : Use quicker tempo change algorithm (gain speed, lose quality)\n"
    "  -naa     : Don't use anti-alias filtering (gain speed, lose quality)\n"
    "  -speech  : Tune algorithm for speech processing (default is for music)\n"
    "  -license : Display the program license text (LGPL)\n";



����ʾ����

soundtouch ../data/qw.wav ../data/1.wav -rate=-10


ע�⣺
1��soundtouch ����д�����ִ�Сд
2��������Ƶ��ʱֻ�� .wav ��ʽ��
