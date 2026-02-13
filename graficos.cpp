#include "graficos.h"
#include "algoritmo.h"
#include <wx/wx.h>

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Programa", wxPoint(50, 50), wxSize(1280, 720)) {
    auto* panel = new wxPanel(this, wxID_ANY);

    auto* cargar_archivo = new wxButton(panel, wxID_ANY, "Cargar Archivo",wxPoint(10, 10), wxSize(150, 30));
    cargar_archivo->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);

    auto* etiqueta1 = new wxStaticText(panel, wxID_ANY, "Semilla: ",wxPoint(10,60));
    etiqueta1->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);

    auto* texbox1 = new wxTextCtrl(panel, wxID_ANY, "1",wxPoint(65,55));
    texbox1->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);

    auto* textbox2 = new wxTextCtrl(panel, wxID_ANY, "",wxPoint(10,90), wxSize(400,550),wxTE_READONLY | wxTE_MULTILINE);
    textbox2->Bind(wxEVT_BUTTON, &MyFrame::OnOpenExplorer, this);
    //comment
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
