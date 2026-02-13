#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
    MyFrame();
    void OnOpenExplorer(const wxCommandEvent& event);
};

#endif
