#ifndef GRAFICADOR_H
#define GRAFICADOR_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

class MyGraphCanvas : public wxPanel {
public:
    MyGraphCanvas(wxWindow* parent, wxPoint pos, wxSize size);
    void OnPaint(wxPaintEvent& event);

    // Nuevos métodos para la GUI
    void SetModo3D(bool activar3D);
    bool Is3D() const { return modo3D; }

private:
    bool modo3D = false; // Estado actual del canvas

    // Separamos la lógica para mayor orden
    void Dibujar2D(wxGraphicsContext* gc, int w, int h);
    void Dibujar3D(wxGraphicsContext* gc, int w, int h);
};

#endif
