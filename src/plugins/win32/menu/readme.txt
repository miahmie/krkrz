���g����

�g���g���y�Ń��j���[(MenuItem, Window.menu)���g�����܂��B
���L�̂悤�ɂ���Window�C���X�^���X�쐬�O�ɖ{�v���O�C���������N���Ă��������B

@if (kirikiriz)
Plugins.link("menu.dll");
@endif

�����N��̊�{�I�ȃ��j���[�̋@�\�͋g���g���Q�Ɠ��l�ł��B


���V���[�g�J�b�g�Ɏw��\�ȕ����ꗗ
���V���[�g�J�b�g�Ƃ��ċ@�\���邩�͕ʖ��ŁA������͖��m�F�̂��ߒ���

Backspace
Tab
Num 5
Enter
Shift
Ctrl
Alt
Esc
�ϊ�
���ϊ�
Space
Page Up
Page Down
End
Home
Left
Up
Right
Down
Sys Req
Insert
Delete
0
1
2
3
4
5
6
7
8
9
A
B
C
D
E
F
G
H
I
J
K
L
M
N
O
P
Q
R
S
T
U
V
W
X
Y
Z
Num 0
Num 1
Num 2
Num 3
Num 4
Num 5
Num 6
Num 7
Num 8
Num 9
Num *
Num +
Num -
Num Del
/
F1
F2
F3
F4
F5
F6
F7
F8
F9
F10
F11
F12
Pause
Scroll Lock
Shift
Shift
Ctrl
Ctrl
Alt
Alt
M
D
C
B
P
Q
J
G
F
:
;
,
-
.
/
@
[
\
]
^
\
Caps Lock
�Ђ炪��
���p/�S�p


���g���g���Q�Ƃ̌݊����ɂ���

�E�t���X�N���[���Ń��j���[�o�[�̎��������^�\���@�\������܂���
��TJS���ŗ\�ߏ�������Ȃǂ̑Ή����s���Ă�������

�E���L�V���[�g�J�b�g�����v���p�e�B���g������܂�

global.MenuItem.textToKeycode = %[ ... ];
global.MenuItem.keycodeToText =  [ ... ];

MenuItem.shortcut�̃V���[�g�J�b�g�L�[�̕�����̕ϊ��e�[�u���ł��B
���z�L�[�R�[�h�ƃe�L�X�g�̑��ݕϊ��Ɏg�p����܂��B
�i�����̕��̓L�[��S���������ɂ��Ďg�p���Ă��������j

textToKeycode[text.toLowerCase()] = VK_*;
keycodeToText[VK_*] = text;

�ڍׂ̓v���O�C�������N��̏�L�����^�z��̒��g���m�F���Ă��������B
���g�����������邱�Ƃ��ł��܂����AMenuItem������ł̕ύX�͍s��Ȃ��ł��������B

�g���g���Q�݊��p�ɗ\�߉��L���ǉ�����Ă��܂��B
textToKeycode["BkSp".toLowerCase()] = VK_BACK;
textToKeycode["PgUp".toLowerCase()] = VK_PRIOR;
textToKeycode["PgDn".toLowerCase()] = VK_NEXT;

�E��L�ϊ��e�[�u���̎d�l�ɂ��AMenuItem.shortcut�̕\�L���ς��ꍇ������܂�
�i������ˉ��z�L�[�R�[�h�����΂P�̂��߁A�ݒ��͐��K������܂��j

	var item = new MenuItem(window, "shortcut normalize test");
	item.shortcut = "Shift+BkSp";
	Debug.message(item.shortcut);
	// -> Shift+Backspace


