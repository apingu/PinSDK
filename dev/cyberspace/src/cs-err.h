//
//
//
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

#pragma once

// 系統錯誤號碼(經常是正確傳ID,錯誤傳負數錯誤碼)
typedef short CSERR_CODE;
#define CSERR_NOT_CONNECTD              0      // 尚未連線
#define CSERR_ACC_SUCCESS               1      // 
#define CSERR_CONNECT_FAILURE          -1      // 連線失敗
#define CSERR_TIMEOUT                  -2      // 超出等待時間
//#define CSERR_ACC_INVALID               -2      // 無此帳號
#define CSERR_DURING_LOGIN             -3    // 正在登入中
#define CSERR_ACC_NOT_EXIST            -4      // 無此帳號
#define CSERR_BAD_PASSWD               -5      // 密碼錯誤
#define CSERR_ALREADY_LOGIN            -6      // 已經登入
#define CSERR_NOT_ALLOW                -7      // 此帳號禁止由此登入(限制型帳號)
#define CSERR_BANNED                   -8      // 此帳號暫停使用,請洽系統管理員
#define CSERR_NOT_LOGIN                -9      // 尚未登入系統(不能進一步作業)
#define CSERR_LEAVING                  -10      // 正在登出中
#define CSERR_POINT_FAILURE            -11      // 點數不足
#define CSERR_LOGOUT_TOO_SHORT         -12      // 登出的時間過短
#define CSERR_EMPTY_JOIN_SERVER        -14      // 無可以轉入的SERVER
#define CSSERR_BAD_MAC                 -15   // 無效的MAC

#define CSERR_CHAR_NOT_EXIST           -20      // 指定的角色不存在
#define CSERR_CHAR_CREATE_FAIL         -21      // 資料庫新增資料失敗
#define CSERR_CHAR_EXIST               -22      // 同名的角色已經存在
#define CSERR_CHAR_BADOWNER            -23      // 此角色不是要求帳號擁有的
#define CSERR_CHAR_BADNAME             -23      // 此角色的名字錯誤

#define CSERR_NOT_ONLINE               -30      // 對象角色未上線
#define CSERR_MESSAGE_BLOCKED          -31      // 訊息被對方阻擋
#define CSERR_NOT_IMPLEMENTED          -32      // 此功能尚未實作
#define CSERR_CREATE_FAIL              -33   // 建立(角色)失敗(service方面失敗)
#define CSERR_DELETE_PLYR_FAIL         -34      // 角色刪除失敗
#define CSERR_CHAR_ENTERED             -35      // 指定角色已經進入系統
#define CSERR_PACKET_MISMATCH          -36      // 接收封包格式不對
#define CSERR_UNKNOWN_ENTITY_TYPE      -37      // 欲建立的 entity 種類不明, 無法建立

#define CSERR_CREATE_ENTITY_ERR        -40      // Server 建立 entity 錯誤
#define CSERR_INVALID_REGION           -41      // 無指定區域; 指定區域不存在

#define CSERR_ACC_REJECT               -42      // 帳號被認證拒絕
#define CSERR_DATABASE_FAILURE         -45      // 資料庫尚未建立


#define CSERR_ACC_UNKNOW_ERROR         -99    // 未知的錯誤
#define CSERR_USER                     -1000  // 使用者定義的(遊戲端)