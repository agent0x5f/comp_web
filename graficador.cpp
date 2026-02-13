#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h> // Importante para wxGraphicsContext
#include "graficador.h"

// Constructor: Implementamos lo que declaramos en el .h
MyGraphCanvas::MyGraphCanvas(wxWindow* parent, wxPoint pos, wxSize size)
    : wxPanel(parent, wxID_ANY, pos, size, wxBORDER_SUNKEN) 
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &MyGraphCanvas::OnPaint, this);
}

// Función de dibujado
void MyGraphCanvas::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        int w, h;
        GetClientSize(&w, &h);
        int margin = 30;

        // Dibujar Ejes
        gc->SetPen(*wxBLACK_PEN);
        gc->StrokeLine(margin, h - margin, w - margin, h - margin); // Eje X
        gc->StrokeLine(margin, margin, margin, h - margin);         // Eje Y

        // Línea de ejemplo
        gc->SetPen(wxPen(wxColour(255, 0, 0), 2));
        gc->StrokeLine(margin, h - margin, w / 2, h / 2);

        delete gc;
    }
}