// Hello World with wxWidgets and c++11
// Original source : https://docs.wxwidgets.org/trunk/overview_helloworld.html
#include <wx/wx.h>

class MyFrame : public wxFrame {
    enum { ID_Hello = 1 };
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, _T("Hello World"))
    {
        wxMenu *menuFile = new wxMenu();
        menuFile->Append(ID_Hello, _T("ハロー(&H)...\tCtrl-H"),
                         _T("ステータスバーにこのメニュー項目の説明を表示します"));
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT);

        wxMenu *menuHelp = new wxMenu();
        menuHelp->Append(wxID_ABOUT);

        wxMenuBar *menuBar = new wxMenuBar();
        menuBar->Append(menuFile, _T("ファイル(&F)"));
        menuBar->Append(menuHelp, _T("ヘルプ(&H)"));

        SetMenuBar( menuBar );

        CreateStatusBar();
        SetStatusText(_T("wxWidgets にようこそ!"));

        Bind(wxEVT_MENU, [](wxCommandEvent& event) {
            wxLogMessage(_T("wxWidgets でハローワールド！"));
        }, ID_Hello);

        Bind(wxEVT_MENU, [](wxCommandEvent& event) {
            wxMessageBox(_T("これは wxWidgets を使った 'Hello World' サンプルです"),
                         _T("Hello World について"), wxOK | wxICON_INFORMATION);
        }, wxID_ABOUT);

        Bind(wxEVT_MENU, [this](wxCommandEvent& event) {
            Close(true);
        }, wxID_EXIT);
    }
};

class MyApp : public wxApp {
public:
    bool OnInit() override {
        MyFrame *frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
