#include <iostream>
#include <limits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"

#include <unordered_map>

using namespace std;

std::vector<state_t> get_children(bool color, state_t state) {

	std::vector<state_t> children;
	state_t new_state;

	for( int pos = 0; pos < DIM; ++pos ) {
		if (state.outflank(color, pos)) {
			new_state = state.move(color, pos);
			children.push_back(new_state);
		}
    }

    return children;
}


int main(int argc, const char **argv) {
    state_t pv[128];
    int npv = 0;
    for( int i = 0; PV[i] != -1; ++i ) ++npv;

    printf("DIM: %d\n", DIM);

    int algorithm = 0;
    if( argc > 1 ) algorithm = atoi(argv[1]);
    bool use_tt = argc > 2;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << flush;
    for( int i = 0; PV[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!
        int pos = PV[i];
        pv[npv - i] = state;
        state = state.move(player, pos);
    }
    pv[0] = state;
    cout << "done!" << endl;

    cout << "Buscando Hijos de " << endl;
    cout << pv[33];

	std::vector<state_t> children = get_children(true, pv[33]);

	int size = children.size();

	cout << "HIJOS" << endl;
	for (int i = 0; i < size; ++i) {
		cout << children[i];
	}

	return 0;

}