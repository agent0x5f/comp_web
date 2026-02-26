#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include "graficador.h"
#include "algoritmo.h"
#include <algorithm>
#include <cmath>

MyGraphCanvas::MyGraphCanvas(wxWindow* parent, wxPoint pos, wxSize size)
    : wxPanel(parent, wxID_ANY, pos, size, wxBORDER_SUNKEN) 
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &MyGraphCanvas::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &MyGraphCanvas::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &MyGraphCanvas::OnMouseLeftUp, this);
    Bind(wxEVT_MOTION, &MyGraphCanvas::OnMouseMotion, this);
}

void MyGraphCanvas::SetModo3D(bool activar3D) {
    modo3D = activar3D;
    Refresh();
}

void MyGraphCanvas::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    int w, h;
    GetClientSize(&w, &h);

    if (modo3D) {
        Dibujar3D(gc, w, h);
    } else {
        Dibujar2D(gc, w, h);
    }

    delete gc;
}

void MyGraphCanvas::OnMouseLeftDown(wxMouseEvent& event) {
    if (modo3D) {
        isDragging = true;
        lastMousePos = event.GetPosition(); 
    }
}

void MyGraphCanvas::OnMouseLeftUp(wxMouseEvent& event) {
    isDragging = false; 
}

void MyGraphCanvas::OnMouseMotion(wxMouseEvent& event) {
    if (isDragging && modo3D) {
        wxPoint currentPos = event.GetPosition();
        
        double deltaX = currentPos.x - lastMousePos.x;
        double deltaY = currentPos.y - lastMousePos.y;
        double sensibilidad = 0.01; 

        angleYaw -= deltaX * sensibilidad;
        anglePitch -= deltaY * sensibilidad;

        if (anglePitch > 1.5) anglePitch = 1.5;
        if (anglePitch < -1.5) anglePitch = -1.5;

        lastMousePos = currentPos;
        Refresh(); 
    }
}

void MyGraphCanvas::Dibujar2D(wxGraphicsContext* gc, int w, int h) {
    int margin = 50;
    int rightMargin = 150;

    double anchoGrafico = w - margin - rightMargin;
