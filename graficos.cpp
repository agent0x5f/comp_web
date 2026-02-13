#include "graficos.h"
#include "algoritmo.h"
#include <wx/wx.h>

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "App Separada", wxPoint(50, 50), wxSize(300, 200)) {
    auto* panel = new wxPanel(this, wxID_ANY);
    auto* button = new wxButton(panel, wxID_ANY, "Seleccionar Archivo",
                                    wxPoint(50, 60), wxSize(200, 30));

    button->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);
    wxFrameBase::CreateStatusBar();
}

void MyFrame::OnOpenExplorer(const wxCommandEvent& event) {
    wxUnusedVar(event);
    // Filtramos para que solo se vean imágenes
    wxFileDialog openFileDialog(this, "Selecciona una imagen", "", "",
                       "Imágenes (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_OK) {
        std::string path = openFileDialog.GetPath().ToStdString();

        // 1. Llamamos al algoritmo y guardamos lo que nos devuelve
        std::string nombreParaMostrar = Algoritmo::procesarImagen(path);

        // 2. Creamos la ventana emergente con el nombre
        wxMessageBox("El algoritmo procesa: " + nombreParaMostrar,"Procesamiento Exitoso",wxOK | wxICON_INFORMATION);
    }
}
