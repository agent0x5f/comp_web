#include <wx/wx.h>
#include "graficos.h"

bool verbo = false; //mostrar todas las calculaciones.

class MyApp : public wxApp {
public:
    bool OnInit() override {
        auto *frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
