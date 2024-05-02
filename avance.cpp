#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

struct Equipo {
    string nombre;
    int puntos;
    int golesMarcados;
    bool eliminado;
};

void jugarPartido(Equipo &equipoLocal, Equipo &equipoVisitante, bool faseEliminatoria) {
    equipoLocal.golesMarcados = rand() % 5;
    equipoVisitante.golesMarcados = rand() % 5;

    // Imprimir el nombre del equipo local y los resultados con el equipo visitante
    cout << equipoLocal.nombre << " " << equipoLocal.golesMarcados << " - " << equipoVisitante.golesMarcados << "   - " << equipoVisitante.nombre << endl;

    if (faseEliminatoria) {
        if (equipoLocal.golesMarcados == equipoVisitante.golesMarcados) {
            // Si hay empate en fase de eliminación, se resuelve por la cantidad de goles en la tanda de penales
            int golesPenalesLocal = rand() % 6; // Genera un número aleatorio entre 0 y 5
            int golesPenalesVisitante = rand() % 6;

            // Mostrar los resultados de la tanda de penales
            cout << "¡Vamos a la tanda de penales!\n";
            cout << "Goles en penales: " << equipoLocal.nombre << " " << golesPenalesLocal << " - " << golesPenalesVisitante << " " << equipoVisitante.nombre << endl;

            if (golesPenalesLocal > golesPenalesVisitante) {
                equipoLocal.puntos += 2; // Se asignan dos puntos por ganar en penales
                equipoVisitante.eliminado = true;
            } else {
                equipoVisitante.puntos += 2;
                equipoLocal.eliminado = true;
            }
        } else {
            // No hay empate, se determina el ganador normalmente
            if (equipoLocal.golesMarcados > equipoVisitante.golesMarcados) {
                equipoLocal.puntos += 3;
                equipoVisitante.eliminado = true;
            } else {
                equipoVisitante.puntos += 3;
                equipoLocal.eliminado = true;
            }
        }
    } else {
        // No es fase de eliminación, solo se actualizan los puntos
        if (equipoLocal.golesMarcados > equipoVisitante.golesMarcados) {
            equipoLocal.puntos += 3;
        } else if (equipoLocal.golesMarcados < equipoVisitante.golesMarcados) {
            equipoVisitante.puntos += 3;
        } else {
            equipoLocal.puntos += 1;
            equipoVisitante.puntos += 1;
        }
    }
}

void simularFaseGrupos(vector<vector<Equipo>> &grupos) {
    for (size_t i = 0; i < grupos.size(); ++i) {
        cout << "---- Grupo " << (i + 1) << " ----\n";

        int numEquipos = grupos[i].size();

        // Asegurar que cada equipo juegue contra todos los demás equipos del grupo
        for (int j = 0; j < numEquipos; ++j) {
            for (int k = j + 1; k < numEquipos; ++k) {
                jugarPartido(grupos[i][j], grupos[i][k], false); // No es fase de eliminación
            }
        }

        // Ordenar los equipos por puntos
        sort(grupos[i].begin(), grupos[i].end(), [](const Equipo &a, const Equipo &b) {
            return a.puntos > b.puntos;
        });

        cout << "\nTabla de posiciones del grupo " << (i + 1) << "\n";
        cout << "Equipo\t\tPuntos\n";
        for (const auto &equipo : grupos[i]) {
            cout << equipo.nombre << "\t\t" << equipo.puntos << "\n";
        }
        cout << "------------------------\n\n";
    }
}

void simularEliminatorias(vector<Equipo> &equipos, const string &nombreRonda, bool faseEliminatoria) {
    cout << "\n  " << nombreRonda << "   \n";

    // Shuffle de los equipos para enfrentamientos más aleatorios
    random_shuffle(equipos.begin(), equipos.end());

    for (int i = 0; i < equipos.size(); i += 2) {
        if (!equipos[i].eliminado && !equipos[i + 1].eliminado) {
            jugarPartido(equipos[i], equipos[i + 1], faseEliminatoria);
        }
    }
}

void generarGrupos(vector<Equipo> &equipos, vector<vector<Equipo>> &grupos) {
    int numGrupos = equipos.size() / 4;
    grupos.resize(numGrupos);

    for (int i = 0; i < numGrupos; ++i) {
        // Ordenar equipos por puntos y luego seleccionar los dos primeros para pasar a cuartos de final
        sort(equipos.begin() + i * 4, equipos.begin() + (i + 1) * 4, [](const Equipo &a, const Equipo &b) {
            return a.puntos > b.puntos;
        });

        grupos[i] = vector<Equipo>(equipos.begin() + i * 4, equipos.begin() + (i + 1) * 4);
    }
}

void simularCopaAmerica() {
    srand(time(0));

    vector<Equipo> equipos;

    ifstream archivoEquipos("equipos.txt");

    if (!archivoEquipos.is_open()) {
        cerr << "No se pudo abrir el archivo de equipos" << endl;
        return;
    }

    string nombreEquipo;
    while (getline(archivoEquipos, nombreEquipo)) {
        equipos.push_back({nombreEquipo, 0, 0, false});
    }

    archivoEquipos.close();

    vector<vector<Equipo>> grupos;
    generarGrupos(equipos, grupos);

    // Simulación de la fase de grupos
    simularFaseGrupos(grupos);

    vector<Equipo> equiposCuartos;
    for (const auto &grupo : grupos) {
        // Agregar los dos primeros equipos de cada grupo a la lista de equipos de cuartos de final
        equiposCuartos.push_back(grupo[0]);
        equiposCuartos.push_back(grupo[1]);
    }

    // Simulación de los cuartos de final
    simularEliminatorias(equiposCuartos, "Cuartos de final", true); // Es fase de eliminación

    // Eliminar equipos perdedores de cuartos de final
    equiposCuartos.erase(remove_if(equiposCuartos.begin(), equiposCuartos.end(), [](const Equipo &equipo) {
        return equipo.eliminado;
    }), equiposCuartos.end());

    // Simulación de las semifinales
    simularEliminatorias(equiposCuartos, "Semifinales", true); // Es fase de eliminación

    // Eliminar equipos perdedores de semifinales
    equiposCuartos.erase(remove_if(equiposCuartos.begin(), equiposCuartos.end(), [](const Equipo &equipo) {
        return equipo.eliminado;
    }), equiposCuartos.end());

    // Simulación de la final
    simularEliminatorias(equiposCuartos, "Final", true); // Es fase de eliminación

    // Mostrar el resultado de la final
    cout << "\n ¡El campeón de la Copa América es: "
         << (equiposCuartos[0].golesMarcados > equiposCuartos[1].golesMarcados ? equiposCuartos[0].nombre : equiposCuartos[1].nombre)
         << "!" << endl;
}

int main() {
    simularCopaAmerica();
    return 0;
}
