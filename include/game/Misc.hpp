#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include "LocalPlayer.hpp"
#include "Offsets.hpp"
#include "Memory.h"
#include "HidTable.hpp"
#include "KmboxB.h"
#include "KmboxNet.hpp"
#include "Aimbot.hpp"

struct Misc {
    // Variables
    LocalPlayer* Myself;
    Aimbot* AimAssist;

    bool SuperGlide;
    bool isTS;
    bool autorbjump;
    bool autoGrpple;
    //检查滞空状态
    bool isOnGround(uint32_t flags)
    {
        return (flags & 0x1) != 0;
    }
    uint32_t flags_old;

    Misc(LocalPlayer* Myself) {
        this->Myself = Myself;
    }

    void Initialize() {
        // idk, nothing for now
    }

    void Update() {
//    {
//        int LP_teamid = mem.Read<int>(Myself->BasePointer + OFF_TEAM_NUMBER);
//
//        int attack_state = mem.Read<int>(mem.OFF_BASE + OFF_INATTACK); // 108
//        int zoom_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_ZOOM); // 109
//        int force_attack_state = mem.Read<int>(mem.OFF_BASE + OFF_INATTACK + 0x8); // 108
//        int force_reload_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_RELOAD + 0x8); // 108
//        std::cout << "attack:" << force_attack_state << " reload:" << force_reload_state << std::endl;
//
//        int jump_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_JUMP); //65 手柄115
//        int force_jump = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8);
//
//        int duck_state = mem.Read<int>(Myself->BasePointer + OFFSET_IN_DUCK); // 玩家下蹲状态 始终0
//        int force_duck = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_DUCK + 0x8); //按住蹲 站立4 蹲下5
//        int tduck_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_TOGGLE_DUCK); // 61 //手柄116
//        int force_toggle_duck = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_TOGGLE_DUCK + 0x8);
//
//        int forward_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_FORWARD); // 前进状态 //33
//        int backward_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_backward); // 后退状态 //29
//        int left_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_moveleft); // 左移状态 //11
//        int right_state = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_moveright); // 右移状态 //14
//
//        int force_forward = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8); // 前进按键 //0?1
//        int force_backward = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8); // 后退按键 //0?1
//        int force_left = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8); // 左移按键 //0?1
//        int force_right = mem.Read<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8); // 右移按键 //0?1
//
//        int flags = mem.Read<int>(Myself->BasePointer + OFFSET_FLAGS); // 玩家空间状态？
//        int wallrun_start = mem.Read<float>(Myself->BasePointer + OFFSET_wallRunStartTime); //爬墙开始 世界时间
//        int wallrun_clear = mem.Read<float>(Myself->BasePointer + OFFSET_wallRunClearTime); //墙上下来 世界时间
//        int skydrive_state = mem.Read<int>(Myself->BasePointer + OFFSET_IN_m_skydiveState); // 跳伞状态 0?1
//
//        int curFrameNumber = mem.Read<int>(mem.OFF_BASE + OFFSET_GlobalVars + 0x0008); // GlobalVars + 0x0008 //当前帧数
//
//        std::cout << "LocalTeamID" << LP_teamid << std::endl;
//        std::cout << "jump_state" << jump_state << std::endl;
//        std::cout << "wallrun_clear" << wallrun_clear << std::endl;
//        std::cout << "right_state" << right_state << std::endl;
//        std::cout << "force_right" << force_right << std::endl;
//    }

        static bool startSg = false;
        if (SuperGlide)
        {
            float worldtime = mem.Read<float>(Myself->BasePointer + OFF_TIME_BASE);						   // Current time
            float traversalStartTime = mem.Read<float>(Myself->BasePointer + OFFSET_TRAVERSAL_START_TIME); // Time to start wall climbing
            float traversalProgress = mem.Read<float>(Myself->BasePointer + OFFSET_TRAVERSAL_PROGRESS);	   // Wall climbing, if > 0.87 it is almost over.
            //static bool startSg = false;
            static float superGlideTimer = 0;
            static float fpsrate = 120;
            auto HangOnWall = -(traversalStartTime - worldtime);
            if (HangOnWall > 0.1 * 60 / fpsrate && HangOnWall < 0.12 * 60 / fpsrate) {
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
            }
            //(jump_state == 65 || jump_state == 115) 跳跃
            if (traversalProgress > 0.87 + 0.02 * fpsrate / 60 && !startSg && HangOnWall > 0.1 * 60 / fpsrate && HangOnWall < 1.5 * 60 / fpsrate && (mem.GetKeyboard()->IsKeyDown(VK_SPACE) || mem.isPressed(65) || mem.isPressed(115))) {
                superGlideTimer = worldtime;
                startSg = true;
            }
            if (startSg)
            {
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 5);

                float currentTime;
                while (true) {
                    if (currentTime = mem.Read<float>(Myself->BasePointer + OFF_TIME_BASE)) {
                        if (currentTime - superGlideTimer < 0.004 * 60 / fpsrate) {
                        }
                        else {
                            break;
                        }
                    }
                }
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_DUCK + 0x8, 6);
                //std::this_thread::sleep_for(std::chrono::milliseconds(20));
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                //std::this_thread::sleep_for(std::chrono::milliseconds(320));

                startSg = false;
            }
            /*if (jump_state == 65 && traversalProgress > 0.5 && worldtime - traversalStartTime < 1)
            {
                if (0.85 > traversalProgress > 0.5 && superGlideTimer == 0 && worldtime - traversalStartTime < 0.4) {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                }
                if (0.95 > traversalProgress > 0.88 < 0.95 && superGlideTimer == 0 && worldtime - traversalStartTime < 0.7) {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 5);
                    superGlideTimer = worldtime;
                }
                if (worldtime - superGlideTimer < 0.004 && superGlideTimer != 0) {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                    superGlideTimer = 0;
                }
            }
            if (1 > worldtime - traversalStartTime > 0.7) {
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_DUCK + 0x8, 6);
            }*/
        }

        //大地图雷达
        if (mem.GetKeyboard()->IsKeyDown(VK_F10) || mem.isPressed(101))
        {
            int RadarID = Myself->Team;
            for (uintptr_t i = 0; i <= 1000; i++)
            {
                mem.Write<int>(Myself->BasePointer + OFF_TEAM_NUMBER, 1);
//                std::cout << "Write TeamNum 1" << std::endl;
            }
            for (uintptr_t i = 0; i <= 1000; i++)
            {
                mem.Write<int>(Myself->BasePointer + OFF_TEAM_NUMBER, RadarID);
//                std::cout << "Write TeamNum:" << radarteamid << std::endl;
            }
        }

        //TS
//        if (isTS)
//        {
//            if (!isOnGround(flags))
//            {
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 0);
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 0);
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 0);
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 0);
//
//                if (forward_state == 33)
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 1);//W
//                }
//                if (backward_state == 29)
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 1);//S
//                }
//                if (left_state == 11)
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 1);//A
//                }
//                if (right_state == 14)
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 1);//D
//                }
//            }
//            else if (isOnGround(flags))
//            {
//                if (forward_state == 33)//W
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 1);
//                }
//                else
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 0);
//                }
//                if (backward_state == 29)//S
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 1);
//                }
//                else
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 0);
//                }
//                if (left_state == 11)//A
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 1);
//                }
//                else
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 0);
//                }
//                if (right_state == 14)//D
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 1);
//                }
//                else
//                {
//                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 0);
//                }
//            }
//        }
//
//        //兔子跳
//        if (autorbjump)
//        {
//            if (isOnGround(flags) && (jump_state == 65 || jump_state == 115) && (force_duck == 61 || tduck_state == 116))
//            {
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 5);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
//            }
//        }

        if (isTS)
        {
            if (!isOnGround(Myself->Flags))
            {

                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 0);
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 0);
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 0);
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 0);

                if (mem.GetKeyboard()->IsKeyDown(0x57) || mem.isPressed(33))
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 1);//W
                }
                else if (mem.GetKeyboard()->IsKeyDown(0x53) || mem.isPressed(29))
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 1);//S
                }
                else if (mem.GetKeyboard()->IsKeyDown(0x41) || mem.isPressed(11))
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 1);//A
                }
                else if (mem.GetKeyboard()->IsKeyDown(0x44) || mem.isPressed(14))
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 1);//D
                }
            }
            else if (isOnGround(Myself->Flags))
            {
                if (mem.GetKeyboard()->IsKeyDown(0x57) || mem.isPressed(33))//W
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 1);
                }
                else
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_FORWARD + 0x8, 0);
                }

                if (mem.GetKeyboard()->IsKeyDown(0x53) || mem.isPressed(29))//S
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 1);
                }
                else
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_backward + 0x8, 0);
                }

                if (mem.GetKeyboard()->IsKeyDown(0x41) || mem.isPressed(11))//A
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 1);
                }
                else
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveleft + 0x8, 0);
                }

                if (mem.GetKeyboard()->IsKeyDown(0x44) || mem.isPressed(14))//D
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 1);
                }
                else
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_moveright + 0x8, 0);
                }
            }
        }

        //兔子跳
        if (autorbjump && !startSg)
        {
            if ((mem.GetKeyboard()->IsKeyDown(VK_SPACE) || mem.isPressed(65)) && isOnGround(Myself->Flags) && (mem.GetKeyboard()->IsKeyDown(VK_CONTROL) || mem.isPressed(83)))
            {
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 5);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
            }
        }



        //自动钩
        if (autoGrpple)
        {
            if (Myself->isGrppleActived > 256)
            {
                if (Myself->isGRAPPLE == 1)
                {
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 5);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_JUMP + 0x8, 4);
                }

            }
        }
    }
};
/*
虚拟键码	 对应值(Hex) 	 对应值(Dec)	对应键
VK_LBUTTON	0x01	1	鼠标左键
VK_RBUTTON	0x02	2	鼠标右键
VK_CANCEL	0x03	3	Cancel
VK_MBUTTON	0x04	4	鼠标中键
VK_XBUTTON1	0x05	5	X1 鼠标按钮(触摸板左键?)
VK_XBUTTON2	0x06	6	X2 鼠标按钮(触摸板右键?)
VK_BACK	0x08	8	Backspace
VK_TAB	0x09	9	Tab
VK_CLEAR	0x0C	12	Clear(NumLock关闭时的数字键盘5)
VK_RETURN	0x0D	13	Enter
VK_SHIFT	0x10	16	Shift
VK_CONTROL	0x11	17	Ctrl
VK_MENU	0x12	18	Alt
VK_PAUSE	0x13	19	Pause
VK_CAPITAL	0x14	20	Caps Lock
VK_KANA	0x15	21	(IME)假名模式
VK_HANGEUL	0x15	21	保持兼容性；使用VK_HANGUL的
VK_HANGUL	0x15	21	(IME)韩文模式
VK_JUNJA	0x17	23	(IME)Junja模式
VK_FINAL	0x18	24	(IME)最终模式
VK_HANJA	0x19	25	(IME)朝鲜语汉字模式
VK_KANJI	0x19	25	(IME)汉字模式
VK_IME_OFF	0x1A	26	(IME)关闭
VK_ESCAPE	0x1B	27	Esc
VK_CONVERT	0x1C	28	(IME)转换
VK_NONCONVERT	0x1D	29	(IME)nonconvert
VK_ACCEPT	0x1E	30	(IME)接受
VK_MODECHANGE	0x1F	31	(IME)模式更改请求
VK_SPACE	0x20	32	Space
VK_PRIOR	0x21	33	Page Up
VK_NEXT	0x22	34	Page Down
VK_END	0x23	35	End
VK_HOME	0x24	36	Home
VK_LEFT	0x25	37	Left Arrow
VK_UP	0x26	38	Up Arrow
VK_RIGHT	0x27	39	Right Arrow
VK_DOWN	0x28	40	Down Arrow
VK_SELECT	0x29	41	Select
VK_PRINT	0x2A	42	Print
VK_EXECUTE	0x2B	43	Execute
VK_SNAPSHOT	0x2C	44	Print Screen
VK_INSERT	0x2D	45	Insert
VK_DELETE	0x2E	46	Delete
VK_HELP	0x2F	47	Help
　	0x30	48	0
　	0x31	49	1
　	0x32	50	2
　	0x33	51	3
　	0x34	52	4
　	0x35	53	5
　	0x36	54	6
　	0x37	55	7
　	0x38	56	8
　	0x39	57	9
　	0x41	65	A
　	0x42	66	B
　	0x43	67	C
　	0x44	68	D
　	0x45	69	E
　	0x46	70	F
　	0x47	71	G
　	0x48	72	H
　	0x49	73	I
　	0x4A	74	J
　	0x4B	75	K
　	0x4C	76	L
　	0x4D	77	M
　	0x4E	78	N
　	0x4F	79	O
　	0x50	80	P
　	0x51	81	Q
　	0x52	82	R
　	0x53	83	S
　	0x54	84	T
　	0x55	85	U
　	0x56	86	V
　	0x57	87	W
　	0x58	88	X
　	0x59	89	Y
　	0x5A	90	Z
VK_LWIN	0x5B	91	Windows键(左)
VK_RWIN	0x5C	92	Windows键(右)
VK_APPS	0x5D	93	应用键(自然科学键盘)
VK_SLEEP	0x5F	95	计算机睡眠
VK_NUMPAD0	0x60	96	小键盘 0
VK_NUMPAD1	0x61	97	小键盘 1
VK_NUMPAD2	0x62	98	小键盘 2
VK_NUMPAD3	0x63	99	小键盘 3
VK_NUMPAD4	0x64	100	小键盘 4
VK_NUMPAD5	0x65	101	小键盘 5
VK_NUMPAD6	0x66	102	小键盘 6
VK_NUMPAD7	0x67	103	小键盘 7
VK_NUMPAD8	0x68	104	小键盘 8
VK_NUMPAD9	0x69	105	小键盘 9
VK_MULTIPLY	0x6A	106	小键盘 *
VK_ADD	0x6B	107	小键盘 +
VK_SEPARATOR	0x6C	108	小键盘 Enter
VK_SUBTRACT	0x6D	109	小键盘 -
VK_DECIMAL	0x6E	110	小键盘 .
VK_DIVIDE	0x6F	111	小键盘 /
VK_F1	0x70	112	F1
VK_F2	0x71	113	F2
VK_F3	0x72	114	F3
VK_F4	0x73	115	F4
VK_F5	0x74	116	F5
VK_F6	0x75	117	F6
VK_F7	0x76	118	F7
VK_F8	0x77	119	F8
VK_F9	0x78	120	F9
VK_F10	0x79	121	F10
VK_F11	0x7A	122	F11
VK_F12	0x7B	123	F12
VK_F13	0x7C	124	F13
VK_F14	0x7D	125	F14
VK_F15	0x7E	126	F15
VK_F16	0x7F	127	F16
VK_F17	0x80	128	F17
VK_F18	0x81	129	F18
VK_F19	0x82	130	F19
VK_F20	0x83	131	F20
VK_F21	0x84	132	F21
VK_F22	0x85	133	F22
VK_F23	0x86	134	F23
VK_F24	0x87	135	F24
VK_NAVIGATION_VIEW	0x88	136	导航UI的视图，参考"Material Design"
VK_NAVIGATION_MENU	0x89	137	导航UI的菜单
VK_NAVIGATION_UP	0x8A	138	导航UI的上移光标键
VK_NAVIGATION_DOWN	0x8B	139	导航UI的下移光标键
VK_NAVIGATION_LEFT	0x8C	140	导航UI的左移光标键
VK_NAVIGATION_RIGHT	0x8D	141	导航UI的右移光标键
VK_NAVIGATION_ACCEPT	0x8E	142	导航UI的接受(跳转)键
VK_NAVIGATION_CANCEL	0x8F	143	导航UI的取消(跳转)键
VK_NUMLOCK	0x90	144	Num Lock
VK_SCROLL	0x91	145	Scroll
VK_OEM_NEC_EQUAL	0x92	146	小键盘'='
VK_OEM_FJ_JISHO	0x92	146	富士通日语(或自动化OA)键盘词典键
VK_OEM_FJ_MASSHOU	0x93	147	Unregister word' key
VK_OEM_FJ_TOUROKU	0x94	148	Register word' key
VK_OEM_FJ_LOYA	0x95	149	Left OYAYUBI' key
VK_OEM_FJ_ROYA	0x96	150	Right OYAYUBI' key
VK_LSHIFT	0xA0	160	上档键(左)
VK_RSHIFT	0xA1	161	上档键(右)
VK_LCONTROL	0xA2	162	Ctrl(左)
VK_RCONTROL	0xA3	163	Ctrl(右)
VK_LMENU	0xA4	164	菜单键(左)
VK_RMENU	0xA5	165	菜单键(右)
VK_BROWSER_BACK	0xA6	166	浏览器后退
VK_BROWSER_FORWARD	0xA7	167	浏览器前进
VK_BROWSER_REFRESH	0xA8	168	浏览器刷新
VK_BROWSER_STOP	0xA9	169	浏览器停止
VK_BROWSER_SEARCH	0xAA	170	浏览器搜索
VK_BROWSER_FAVORITES	0xAB	171	浏览器收藏夹
VK_BROWSER_HOME	0xAC	172	浏览器起始页或主页
VK_VOLUME_MUTE	0xAD	173	音量静音
VK_VOLUME_DOWN	0xAE	174	音量降低
VK_VOLUME_UP	0xAF	175	音量升高
VK_MEDIA_NEXT_TRACK	0xB0	176	下一曲目
VK_MEDIA_PREV_TRACK	0xB1	177	上一曲目
VK_MEDIA_STOP	0xB2	178	停止媒体
VK_MEDIA_PLAY_PAUSE	0xB3	179	播放/暂停媒体
VK_LAUNCH_MAIL	0xB4	180	启动邮件
VK_LAUNCH_MEDIA_SELECT	0xB5	181	选择媒体
VK_LAUNCH_APP1	0xB6	182	1键启动应用程序
VK_LAUNCH_APP2	0xB7	183	2键启动应用程序
VK_OEM_1	0xBA	186	; :
VK_OEM_PLUS	0xBB	187	= +
VK_OEM_COMMA	0xBC	188	<,
VK_OEM_MINUS	0xBD	189	- _
VK_OEM_PERIOD	0xBE	190	>.
VK_OEM_2	0xBF	191	/ ?
VK_OEM_3	0xC0	192	` ~
VK_GAMEPAD_A	0xC3	195	游戏手柄 A
VK_GAMEPAD_B	0xC4	196	游戏手柄 B
VK_GAMEPAD_X	0xC5	197	游戏手柄 X
VK_GAMEPAD_Y	0xC6	198	游戏手柄 Y
VK_GAMEPAD_RIGHT_SHOULDER	0xC7	199	侧顶部R1键
VK_GAMEPAD_LEFT_SHOULDER	0xC8	200	侧顶部L1键
VK_GAMEPAD_LEFT_TRIGGER	0xC9	201	侧顶部L2键
VK_GAMEPAD_RIGHT_TRIGGER	0xCA	202	侧顶部R2键
VK_GAMEPAD_DPAD_UP	0xCB	203	十字方向键盘上键
VK_GAMEPAD_DPAD_DOWN	0xCC	204	十字方向键盘下键
VK_GAMEPAD_DPAD_LEFT	0xCD	205	十字方向键盘左键
VK_GAMEPAD_DPAD_RIGHT	0xCE	206	十字方向键盘右键
VK_GAMEPAD_MENU	0xCF	207	菜单键(图标以三横杠表示)
VK_GAMEPAD_VIEW	0xD0	208	查看键(图标以两个叠加方框表示)
VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON	0xD1	209	L3键(左摇杆垂直按下)
VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON	0xD2	210	R3键(右摇杆垂直按下)
VK_GAMEPAD_LEFT_THUMBSTICK_UP	0xD3	211	左摇杆方向上键
VK_GAMEPAD_LEFT_THUMBSTICK_DOWN	0xD4	212	左摇杆方向下键
VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT	0xD5	213	左摇杆方向右键
VK_GAMEPAD_LEFT_THUMBSTICK_LEFT	0xD6	214	左摇杆方向左键
VK_GAMEPAD_RIGHT_THUMBSTICK_UP	0xD7	215	右摇杆方向上键
VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN	0xD8	216	右摇杆方向下键
VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT	0xD9	217	右摇杆方向右键
VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT	0xDA	218	右摇杆方向左键
VK_OEM_4	0xDB	219	[ {
VK_OEM_5	0xDC	220	\|
VK_OEM_6	0xDD	221	] }
VK_OEM_7	0xDE	222	' "
VK_OEM_8	0xDF	223	§ !
VK_OEM_AX	0xE1	225	日本键盘'AX'键
VK_OEM_102	0xE2	226	"<>"或RT 102键键盘上的"\|"
VK_ICO_HELP	0xE3	227	IcoHlp
VK_ICO_00	0xE4	228	Ico00
VK_PROCESSKEY	0xE5	229	Process
VK_ICO_CLEAR	0xE6	230	IcoClr
VK_PACKET	0xE7	231	将Unicode字符作为击键传递
VK_OEM_RESET	0xE9	233	Reset
VK_OEM_JUMP	0xEA	234	Jump
VK_OEM_PA1	0xEB	235	Pa1
VK_OEM_PA2	0xEC	236	Pa2
VK_OEM_PA3	0xED	237	Pa3
VK_OEM_WSCTRL	0xEE	238	WsCtrl
VK_OEM_CUSEL	0xEF	239	Cu Sel
VK_OEM_ATTN	0xF0	240	Oem Attn
VK_OEM_FINISH	0xF1	241	Finish
VK_OEM_COPY	0xF2	242	Copy
VK_OEM_AUTO	0xF3	243	Auto
VK_OEM_ENLW	0xF4	244	Enlw
VK_OEM_BACKTAB	0xF5	245	Back Tab
VK_ATTN	0xF6	246	联系人键
VK_CRSEL	0xF7	247	CRSEL键
VK_EXSEL	0xF8	248	ExSel键
VK_EREOF	0xF9	249	删除EOF键
VK_PLAY	0xFA	250	播放键
VK_ZOOM	0xFB	251	变焦键
VK_NONAME	0xFC	252	NoName
VK_PA1	0xFD	253	PAI键
VK_OEM_CLEAR	0xFE	254	清除键

*/