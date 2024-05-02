#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

struct Equipo {
    string nombre;
    string estadio;
    int puntos;
    int golesMarcados;
    bool eliminado;
};

void jugarPartido(Equipo &equipoLocal, Equipo &equipoVisitante, const string &estadio, bool faseEliminatoria) {
    equipoLocal.golesMarcados = rand() % 5;
    equipoVisitante.golesMarcados = rand() % 5;

    cout << equipoLocal.nombre << " " << equipoLocal.golesMarcados << " - " << equipoVisitante.golesMarcados << " " << equipoVisitante.nombre << "   - " << estadio << endl;

    if (faseEliminatoria) {
        if (equipoLocal.golesMarcados == equipoVisitante.golesMarcados) {
            // En caso de empate, jugamos una tanda de penales hasta que haya un ganador
            while (equipoLocal.golesMarcados == equipoVisitante.golesMarcados) {
                equipoLocal.golesMarcados = rand() % 6; // Nuevo resultado de penales para el equipo local
                equipoVisitante.golesMarcados = rand() % 6; // Nuevo resultado de penales para el equipo visitante
            }

            cout << "¡Vamos a la tanda de penales!\n";
            cout << "Goles en penales: " << equipoLocal.nombre << " " << equipoLocal.golesMarcados << " - " << equipoVisitante.golesMarcados << " " << equipoVisitante.nombre << endl;

            if (equipoLocal.golesMarcados > equipoVisitante.golesMarcados) {
                equipoLocal.puntos += 2;
                equipoVisitante.eliminado = true;
            } else {
                equipoVisitante.puntos += 2;
                equipoLocal.eliminado = true;
            }
        } else {
            if (equipoLocal.golesMarcados > equipoVisitante.golesMarcados) {
                equipoLocal.puntos += 3;
                equipoVisitante.eliminado = true;
            } else {
                equipoVisitante.puntos += 3;
                equipoLocal.eliminado = true;
            }
        }
    } else {
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

        for (int j = 0; j < numEquipos; ++j) {
            for (int k = j + 1; k < numEquipos; ++k) {
                jugarPartido(grupos[i][j], grupos[i][k], grupos[i][j].estadio, false);
            }
        }

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

    random_shuffle(equipos.begin(), equipos.end());

    for (int i = 0; i < equipos.size(); i += 2) {
        if (!equipos[i].eliminado && !equipos[i + 1].eliminado) {
            jugarPartido(equipos[i], equipos[i + 1], equipos[i].estadio, faseEliminatoria);
        }
    }
}

void generarGrupos(vector<Equipo> &equipos, vector<vector<Equipo>> &grupos) {
    int numGrupos = equipos.size() / 4;
    grupos.resize(numGrupos);

    for (int i = 0; i < numGrupos; ++i) {
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
    vector<string> nombresEstadios = {"Allegiant Stadium", "Las Vegas", "Nevada", "AT&T Stadium", " Arlington", " Texas", "Bank of America Stadium", " Charlotte", "North Carolina", "Children's Mercy Park", " Kansas City", "Kansas"};

    while (getline(archivoEquipos, nombreEquipo)) {
        string nombreEstadio = nombresEstadios[rand() % nombresEstadios.size()];
        equipos.push_back({nombreEquipo, nombreEstadio, 0, 0, false});
    }

    archivoEquipos.close();

    vector<vector<Equipo>> grupos;
    generarGrupos(equipos, grupos);

    simularFaseGrupos(grupos);

    vector<Equipo> equiposCuartos;
    for (const auto &grupo : grupos) {
        equiposCuartos.push_back(grupo[0]);
        equiposCuartos.push_back(grupo[1]);
    }

    simularEliminatorias(equiposCuartos, "Cuartos de final", true);

    equiposCuartos.erase(remove_if(equiposCuartos.begin(), equiposCuartos.end(), [](const Equipo &equipo) {
        return equipo.eliminado;
    }), equiposCuartos.end());

    simularEliminatorias(equiposCuartos, "Semifinales", true);

    equiposCuartos.erase(remove_if(equiposCuartos.begin(), equiposCuartos.end(), [](const Equipo &equipo) {
        return equipo.eliminado;
    }), equiposCuartos.end());

    simularEliminatorias(equiposCuartos, "Final", true);

    cout << "\n ¡El campeón de la Copa América es: "
         << (equiposCuartos[0].golesMarcados > equiposCuartos[1].golesMarcados ? equiposCuartos[0].nombre : equiposCuartos[1].nombre)
         << "!" << endl;
}

int main() {
    simularCopaAmerica();
    return 0;
}
