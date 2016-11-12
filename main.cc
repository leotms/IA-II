// Game of Othello -- Example of main
// Universidad Simon Bolivar, 2012.
// Author: Blai Bonet
// Last Revision: 1/11/16
// Modified by:
//  - Leonardo Martinez
//  - Nicolas Manan
//  - Joel Rivas
// Last Modification: 11/11/16

#include <iostream>
#include <limits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"
#include <climits>

#include <unordered_map>

using namespace std;

unsigned expanded = 0;
unsigned generated = 0;
int tt_threshold = 32; // threshold to save entries in TT

// Transposition table
struct stored_info_t {
    int value_;
    int type_;
    enum { EXACT, LOWER, UPPER };
    stored_info_t(int value = -100, int type = LOWER) : value_(value), type_(type) { }
};

struct hash_function_t {
    size_t operator()(const state_t &state) const {
        return state.hash();
    }
};

class hash_table_t : public unordered_map<state_t, stored_info_t, hash_function_t> {
};

hash_table_t TTable[2];

// Given a state and a player, returns all the children states,
// wich represent all moves the player can do.
std::vector<state_t> get_children(state_t state, bool player) {

    std::vector<state_t> children;
    state_t new_state;

    for( int pos = 0; pos < DIM; ++pos ) {
        // outflank moves are the only allowed.
        if (state.outflank(player, pos)) {
            new_state = state.move(player, pos);
            children.push_back(new_state);
        }
    }

    return children;
}

// Forward definition for maxmin algorithm
int maxmin(state_t state, int depth, bool use_tt);

// Definition of MINMAX algorithm, this definition does not transposition tables.
int minmax(state_t state, int depth, bool use_tt = false) {
    if (state.terminal()) {
        return state.value();
    }

    int score = INT_MAX;
    bool player = false; // MIN player is always white

    ++expanded;

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;
        score = min(score, maxmin(child, depth + 1, false));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        score = maxmin(state, depth + 1, false);
    }

    return score;
}

// Definition of MAXMIN algorithm, this definition does not transposition tables.
int maxmin(state_t state, int depth, bool use_tt = false) {
    if (state.terminal()) {
        return state.value();
    }

    int  score = INT_MIN;
    bool player = true;  // MAX player is always black

    ++expanded;

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;
        score = max(score, minmax(child, depth + 1, false));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        score = minmax(state, depth + 1, false);
    }

    return score;
}

// Definition of NEGAMAX algorithm.
// This definition does not transposition tables.
int negamax(state_t state, int depth, int color, bool use_tt = false) {
    if (state.terminal()) {
        return color * state.value();
    }

    // true for black player
    // false for white player
    // color 1 is for black player
    bool player = color == 1;

    ++expanded;

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    int alpha = INT_MIN;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;
        alpha = max(alpha, -negamax(child, depth + 1, -color));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        alpha = max(alpha, -negamax(state, depth + 1, -color));
    }

    return alpha;
}

// Implementation of NEGAMAX with alpha-beta pruning. It does not
// use transposition tables.
int negamax(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false){
    if (state.terminal()) {
        return color * state.value();
    }

    // true for black player
    // false for white player
    // color 1 is for black player
    bool player = color == 1;

    ++expanded;

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    int score = INT_MIN;

    for (int i = 0; i < nchildren; ++i) {
        child =  children[i];
        ++generated;
        int val = -negamax(child, depth + 1, -beta, -alpha, -color, false);
        score = max(score, val);
        alpha = max(alpha, val);
        if(alpha >= beta){
            break;
        }
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
      int val = -negamax(state, depth + 1, -beta, -alpha, -color, false);
      score = max(score, val);
    }

    return score;
}

// Implements TEST value of a state. Used for SCOUT algorithm.
// Conditions:
//    0 : >
//    1 : >=
bool test(state_t state, int depth, int score, int color, int condition){
    if (state.terminal()) {
        if (condition == 0) {
            return state.value() > score ? true : false;
        } else if(condition == 1) {
            return state.value() >= score ? true : false;
        }
    }

    bool player = color == 1;

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {

        child = children[i];

        //state is a MAX state
        if (color == 1  && test(child, depth + 1, score, -color, condition)){
            return true;
        }
        //state is a MIN state
        if (color == -1 && !test(child, depth + 1, score, -color, condition)){
            return false;
        }
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        //state is a MAX state
        if (color == 1  && test(state, depth + 1, score, -color, condition)){
            return true;
        }
        //state is a MIN state
        if (color == -1 && !test(state, depth + 1, score, -color, condition)){
            return false;
        }
    }

    return color == 1 ? false : true;

}

// Implementation of SCOUT algorithm. It does not
// use transposition tables.
int scout(state_t state, int depth, int color, bool use_tt = false) {
    if (state.terminal()){
        return state.value();
    }

    int  score = 0;
    bool player = color == 1;

    ++expanded;

    // we get the clindren states of state for player's turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;

        // check the first child to get the bound
        if (i == 0) {
            score = scout(child, depth + 1, -color, false);
        } else {
            //State is a MAX state.
            if (color == 1 && test(child, depth + 1, score, -color, 0)){
                score = scout(child, depth + 1, -color, false);
            }
            //State is a MIN state.
            if (color == -1 && !test(child, depth + 1, score, -color, 1)){
                score = scout(child, depth + 1, -color, false);
            }
        }
    }

    //The player can't make moves, so we check the other player's turn
    if (nchildren == 0) {
      score = scout(state, depth + 1, -color, false);
    }

    return score;
}

// Implementation of NEGASCOUT algorithm. It does not
// use transposition tables.
int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false) {
    if (state.terminal()){
        return color * state.value();
    }

    int score;
    bool player = color == 1;

    ++expanded;

    // we get the clindren states of state for player's turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {

        child = children[i];
        ++generated;

        // check the first child to get the bound
        if (i == 0) {
            score = -negascout(child, depth + 1, -beta, -alpha, -color);
        } else {
            score = -negascout(child, depth + 1, -alpha - 1, -alpha, -color);

            if ((alpha < score)&&(score < beta)) {
                score = -negascout(child, depth + 1, -beta, -score, -color);
            }
        }

        alpha = max(alpha, score);

        if (alpha >= beta){
            break;
        }

    }

    //The player can't make moves, so we check the other player's turn
    if (nchildren == 0) {
      score = -negascout(state, depth + 1, -beta, -alpha, -color);
      alpha = max(alpha, score);
    }

    return alpha;
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

#if 0
    // print principal variation
    for( int i = 0; i <= npv; ++i )
        cout << pv[npv - i];
#endif

    // Print name of algorithm
    cout << "Algorithm: ";
    if( algorithm == 0 ) {
        cout << "Minmax-Maxmin";
    } else if( algorithm == 1 ) {
        cout << "Negamax (minmax version)";
    } else if( algorithm == 2 ) {
        cout << "Negamax (alpha-beta version)";
    } else if( algorithm == 3 ) {
        cout << "Scout";
    } else if( algorithm == 4 ) {
        cout << "Negascout";
    }
    cout << (use_tt ? " w/ transposition table" : "") << endl;

    // Run algorithm along PV (backwards)
    cout << "Moving along PV:" << endl;
    for( int i = 0; i <= npv; ++i ) {
        //cout << pv[i];
        int value = 0;
        TTable[0].clear();
        TTable[1].clear();
        float start_time = Utils::read_time_in_seconds();
        expanded = 0;
        generated = 0;
        int color = i % 2 == 1 ? 1 : -1;

        try {
            if( algorithm == 0 ) {
                value = color * (color == 1 ? maxmin(pv[i], 0, use_tt) : minmax(pv[i], 0, use_tt));
            } else if( algorithm == 1 ) {
                value = negamax(pv[i], 0, color, use_tt);
            } else if( algorithm == 2 ) {
                value = negamax(pv[i], 0, -200, 200, color, use_tt);
            } else if( algorithm == 3 ) {
                value = color * scout(pv[i], 0, color, use_tt);
            } else if( algorithm == 4 ) {
                value = negascout(pv[i], 0, -200, 200, color, use_tt);
            }
        } catch( const bad_alloc &e ) {
            cout << "size TT[0]: size=" << TTable[0].size() << ", #buckets=" << TTable[0].bucket_count() << endl;
            cout << "size TT[1]: size=" << TTable[1].size() << ", #buckets=" << TTable[1].bucket_count() << endl;
            use_tt = false;
        }

        float elapsed_time = Utils::read_time_in_seconds() - start_time;

        cout << npv + 1 - i << ". " << (color == 1 ? "Black" : "White") << " moves: "
             << "value=" << color * value
             << ", #expanded=" << expanded
             << ", #generated=" << generated
             << ", seconds=" << elapsed_time
             << ", #generated/second=" << generated/elapsed_time
             << endl;
    }

    return 0;
}
