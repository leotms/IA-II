// This file contains the a copy of the implemented algoritms as
// implemented in main.cc. It is not requiered or used by main.cc
// Authors:
//  - Leonardo Martinez
//  - Nicolas Manan
//  - Joel Rivas
// Last Modification: 14/11/16

// Forward definition for maxmin algorithm
int maxmin(state_t state, int depth, bool use_tt);

// Definition of MINMAX algorithm, this definition does not transposition tables.
int minmax(state_t state, int depth, bool use_tt = false) {
    if (state.terminal()) {
        return state.value();
    }

    int score = INT_MAX;
    bool player = false; // MIN player is always white

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    ++expanded;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;
        score = min(score, maxmin(child, depth + 1, false));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        ++generated;
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

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    ++expanded;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;
        score = max(score, minmax(child, depth + 1, false));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        ++generated;
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

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    ++expanded;

    int alpha = INT_MIN;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        ++generated;
        alpha = max(alpha, -negamax(child, depth + 1, -color));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        ++generated;
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

    // We get the children of the state for the player turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    ++expanded;

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
      ++generated;
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

    ++expanded;

    for (int i = 0; i < nchildren; ++i) {

        child = children[i];
        ++generated;

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
        ++generated;
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
      ++generated;
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

    // we get the clindren states of state for player's turn
    std::vector<state_t> children = get_children(state, player);
    int nchildren = children.size();
    state_t child;

    ++expanded;

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
      ++generated;
      score = -negascout(state, depth + 1, -beta, -alpha, -color);
      alpha = max(alpha, score);
    }

    return alpha;
}
