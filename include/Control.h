#pragma once

/*
 * 此处定义 PlayerUI 中的操作方式
 */

/* 下一曲 */
#define CNEXT 'n'

/* 上一曲 */
#define CPREV 'b'

/* 暂停/继续 */
#define CPAUSE 'p'

/* 退出 */
#define CEXIT 'q'

/* 快退 */
#define CSEEKL koll::Keyboard::LEFT

/* 快进 */
#define CSEEKR koll::Keyboard::RIGHT

/* 切换播放模式 */
#define CMODE 'z'

/* 播放列表 下一页 */
#define CPAGENEXT 'v'

/* 播放列表 上一页 */
#define CPAGEPREV 'c'

/* 隐藏/显示 帮助 */
#define CHELP 'h'

/* 隐藏/显示 播放列表 */
#define CPLAYLIST 'g'


/* 默认显示帮助 */
#define DEFAULT_SHOW_HELP true

/* 默认显示播放列表 */
#define DEFAULT_SHOW_PLAY_LIST true

/* 默认播放模式: 列表循环 */
#define DEFAULT_PLAY_MODE LOOP_ALL
