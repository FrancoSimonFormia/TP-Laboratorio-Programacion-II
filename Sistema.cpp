#include "Sistema.h"
#include <filesystem>
#include <iostream>
#include <vector>
using namespace std;
#include "Menu.h"
#include "func_utiles.h"
#include "func_archivos.h"

bool crearDirectorios(string ruta)
{
    return filesystem::create_directories(ruta);
}

Sistema::Sistema() :
    _gestorCarrera("Archivos/datos/carrera.dat","carga_inicial.dat"),
    _gestorEventos("Archivos/datos/eventos.dat", "Archivos/datos/materias.dat", "Archivos/datos/cursada_materias.dat" ),
    _gestorMaterias("Archivos/datos/materias.dat"),
    _gestorNotasFinales("Archivos/datos/notas.dat"),
    _cargaInicial("carga_inicial.dat"),
    _gestorCuatrimestre("Archivos/datos/cuatrimestre.dat"),
    _gestorCsv("archivoImportacion.csv", "Archivos/datos/materias.dat", "carga_inicial.dat")
{
    //ctor
}

void Sistema::iniciar()
{
//    system("color B1");
    crearDirectoriosEsenciales();

    int datosAgregadoss=0;
    bool materiaCargada = false, carreraCargada =false;

    if(_cargaInicial.archivoExiste())
        {
            CargaInicial datos;
            _cargaInicial.leerRegistro(0,datos);

            datosAgregadoss=datos.getdatoscargados();
            materiaCargada = datos.getMateriasCargadas();
            carreraCargada = datos.getCarreraCargada();

        }
    else
        {
            _cargaInicial.crearArchivo();
        }


    string mensaje = "Datos necesarios para el funcionamiento del sistema:\n";
    mensaje += carreraCargada ? "Carrera : OK" : "Carrera : Falta";
    mensaje += "\n";
    mensaje += materiaCargada ? "Materias : OK" : "Materias : Falta";
    mensaje += "\n";


    ///Si falta 1 o  los 2 datos entra el bucle de carga inicial
    if(!materiaCargada || !carreraCargada)
        {
            menuCargaInicial();
        }

    CargaInicial cargaInicial;
    _cargaInicial.leerRegistro(0,cargaInicial);


    if (!cargaInicial.getMateriasCargadas() || !cargaInicial.getCarreraCargada())
        {
            _mensajero.mensajeAdvertencia("No se puede continuar hasta no finalizar la carga inicial de datos. Reiniciar el programa.");
            return;
        }

    // Chequeo eventos pr�ximos
    string alertaEvento = "";
    // TODO : leer limite de d�as de CONFIG
    int diasDeChequeoEventosProximos = 8;
    if (_gestorEventos.hayEventoEnLosProximosDias(diasDeChequeoEventosProximos))
        {
            alertaEvento = " (!)";
        }

    vector <string> opcMenu = {"Materias",
                               "Cuatrimestres cursados",
                               "Eventos" + alertaEvento,
                               "Notas finales",
                               "Carrera",
                               "Configuracion"
                              };

    Menu menu(opcMenu, "Sistema de Gestion de Carrera Universitaria");
    int opc;

    /// Una vez finalizada la carga inicial
    while(true)
        {


            opc = menu.mostrar();
            switch(opc)
                {

                case 0:
                    return;
                    break;
                case 1:
                {
                    _gestorMaterias.iniciarGestorMaterias();
                }
                case 2:
                {
                    _gestorCuatrimestre.iniciarGestorCuatrimestre();
                }
                break;
                case 3:
                    _gestorEventos.iniciar();
                    break;
                case 4:
                    _gestorNotasFinales.iniciar();
                    break;
                case 5:
                    _gestorCarrera.iniciar();
                    break;
                case 6:
                {
                    cout<<" En desarrollo "<<endl;
                }
                break;
                default:
                    break;
                }
        }

}


void Sistema::crearDirectoriosEsenciales()
{

    crearDirectorios("Archivos/datos");
    crearDirectorios("Archivos/configuracion");
    crearDirectorios("Archivos/cursada");

    return;
}

void Sistema::menuCargaInicial()
{
    CargaInicial datos;
    _cargaInicial.leerRegistro(0,datos);

    bool materiaCargada = datos.getMateriasCargadas();
    bool carreraCargada = datos.getCarreraCargada();

    string mensaje = "Datos necesarios para el funcionamiento del sistema:\n";
    mensaje += carreraCargada ? "Carrera : OK" : "Carrera : Falta";
    mensaje += "\n";
    mensaje += materiaCargada ? "Materias : OK" : "Materias : Falta";
    mensaje += "\n";

    _mensajero.mensajeInformacion(mensaje);


    _mensajero.mensajeInformacion("Como primer paso, se debe cargar la informaci�n de la carrera y de todas las materias de la misma.");

    vector <string> opcMenuInicial = {"> Cargar datos de la carrera ","> Cargar de forma manual las materias", "> Cargar las materias mediante archivo csv "};
    Menu menuInicial(opcMenuInicial, "MENU DE CARGA INICIAL");

    bool salir=false;

    while(!materiaCargada || !carreraCargada )
        {
            if (salir) break;

            int opc = menuInicial.mostrar();
            limpiarPantalla();

            switch(opc)
                {

                case 0:
                    salir = true;
                    break;
                case 1:
                {
                    if(!carreraCargada)
                        {
                            _gestorCarrera.cargarManual();
                            CargaInicial datos;
                            _cargaInicial.leerRegistro(0,datos);
                            datos.aumentarcontadorDatosCargados();
                            datos.setCarreraCargada(true);
                            _cargaInicial.modificarRegistro(0,datos);
                        }
                    else
                        {
                            _mensajero.mensajeInformacion("La carrera ya fue cargada correctamente. Ingresar datos de la carrera.");
                            CargaInicial datos;
                            _cargaInicial.leerRegistro(0,datos);
                            datos.setCarreraCargada(true);
                            _cargaInicial.modificarRegistro(0,datos);
                        }
                    //limpiarPantalla();
                    break;
                }
                case 2:
                {
                    if (materiaCargada)
                        {
                            _mensajero.mensajeInformacion("Las materias ya fueron cargadas correctamente. Ingresar datos de la carrera.");
                            break;
                        }

                    _gestorMaterias.iniciarGestorMaterias();

                    std::cout << "Luego de confirmar la carga actual, no podr�n agregarse nuevas materias." << std::endl;
                    std::cout << "Si selecciona 'N', se guardar� informaci�n parcial de las materias, permitiendo continuar luego." << std::endl;
                    std::cout << "Desea dar por finalizada la carga de todas materias? (S/N) " << std::endl;
                    std::cin.clear();

                    char respuesta;
                    respuesta = validar<char>();
                    if(respuesta=='S' || respuesta=='s')
                        {
                            _mensajero.mensajeInformacion("Se ha guardado la informaci�n total de las materias.\nPara el correcto funcionamiento del sistema, no pueden agregarse m�s materias.");
                            CargaInicial datos;
                            _cargaInicial.leerRegistro(0,datos);
                            datos.aumentarcontadorDatosCargados();
                            datos.setMateriasCargadas(true);
                            _cargaInicial.modificarRegistro(0,datos);
                            break;
                        }
                    else
                        {
                            _mensajero.mensajeInformacion("Se ha guardado informaci�n parcial de las materias.\nA�n pueden agregarse m�s materias.");
                        }

                    break;
                }


                case 3:
                {
                    Archivo<Materia> Materias("Archivos/datos/materias.dat");
                    if(Materias.archivoExiste())
                        {
                            std::cout<<" Hemos detectado que ya realizo ingresos manuales de materias"<<std::endl;
                            std::cout<<" en caso de querer ingresar las materias a travez de la carga"<<std::endl;
                            std::cout<<" por archivo, se eliminara toda la informacion anteriormente ingresada "<<std::endl;
                            std::cout<<" para evitar errores"<<std::endl;
                            std::cout<<" Desea Continuar? (S/N) "<<std::endl;
                            char res = validar<char>();

                            if(res =='s' || res =='S' )
                                {
                                    CargaInicial datos;
                                    _cargaInicial.leerRegistro(0,datos);
                                    datos.disminuirDatosCargados();
                                    datos.setMateriasCargadas(false);
                                    _cargaInicial.modificarRegistro(0,datos);
                                    Materias.crearArchivo();
                                    _gestorCsv.iniciar();

                                }
                        }

                    else
                        {
                            _gestorCsv.iniciar();
                        }


                    break;
                }

                default:
                    break;
                }
        }
}

