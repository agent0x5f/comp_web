#ifndef GRAFICADOR_H
#define GRAFICADOR_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

class MyGraphCanvas : public wxPanel {
public:
    MyGraphCanvas(wxWindow* parent, wxPoint pos, wxSize size);
    void OnPaint(wxPaintEvent& event);

    // Aquí puedes añadir variables para tus datos después
    // std::vector<double> datos;
};

#endif