#include <iostream>
#include <limits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"
#include <climits>

#include <unordered_map>

using namespace std;

std::vector<state_t> get_children(state_t state, bool player) {

    std::vector<state_t> children;
    state_t new_state;

    for( int pos = 0; pos < DIM; ++pos ) {
        if (state.outflank(player, pos)) {
            new_state = state.move(player, pos);
            children.push_back(new_state);
        }
    }

    return children;
}

int main(int argc, const char **argv) {
    state_t pv[128];
    int npv = 0;
    for( int i = 0; PV[i] != -1; ++i ) ++npv;

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

    cout << "CONTANDO" << endl;

	cout << pv[0]  << endl;
	cout << pv[0].value() << endl;

	cout << "CONTANDO" << endl;

    for (int i = 0; i < DIM; ++i) {
	    cout << pv[i]  << endl;
	    cout << pv[i].value() << endl;
    }

    return 0;
}
