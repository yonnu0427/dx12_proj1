#include "Window.h"
#include "Input.h"
#include <tchar.h>

//面倒だけど書かなあかんやつ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY) {//ウィンドウが破棄されたら呼ばれます
        PostQuitMessage(0);//OSに対して「もうこのアプリは終わるんや」と伝える
        return 0;
    }

    if (msg == WM_INPUT) {
        Input::ReceiveRawInput(lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}

const unsigned int window_width = 1280;
const unsigned int window_height = 720;

void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass) {
    HINSTANCE hInst = GetModuleHandle(nullptr);
    //ウィンドウクラス生成＆登録
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバック関数の指定
    windowClass.lpszClassName = _T("DirectXTest");//アプリケーションクラス名(適当でいいです)
    windowClass.hInstance = GetModuleHandle(0);//ハンドルの取得
    RegisterClassEx(&windowClass);//アプリケーションクラス(こういうの作るからよろしくってOSに予告する)

    RECT wrc = { 0,0, window_width, window_height };//ウィンドウサイズを決める
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//ウィンドウのサイズはちょっと面倒なので関数を使って補正する
    //ウィンドウオブジェクトの生成
    hwnd = CreateWindow(windowClass.lpszClassName,//クラス名指定
        _T("DX12 マテリアル反映"),//タイトルバーの文字
        WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウです
        CW_USEDEFAULT,//表示X座標はOSにお任せします
        CW_USEDEFAULT,//表示Y座標はOSにお任せします
        wrc.right - wrc.left,//ウィンドウ幅
        wrc.bottom - wrc.top,//ウィンドウ高
        nullptr,//親ウィンドウハンドル
        nullptr,//メニューハンドル
        windowClass.hInstance,//呼び出しアプリケーションハンドル
        nullptr);//追加パラメータ
}