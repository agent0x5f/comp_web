#include <wx/wx.h>
#include "graficos.h"

class MyApp : public wxApp {
public:
    bool OnInit() override {
        auto *frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
