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
        score = min(score, maxmin(child, depth - 1, false));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        score = maxmin(state, depth - 1, false);
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
        score = max(score, minmax(child, depth - 1, false));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        score = minmax(state, depth - 1, false);
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
        alpha = max(alpha, -negamax(child, depth - 1, -color));
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        alpha = max(alpha, -negamax(state, depth - 1, -color));
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
        int val = -negamax(child, depth - 1, -beta, -alpha, -color, false);
        score = max(score, val);
        alpha = max(alpha, val);
        if(alpha >= beta){
            break;
        }
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
      int val = -negamax(state, depth - 1, -beta, -alpha, -color, false);
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
        if (color == 1  && test(child, depth - 1, score, -color, condition)){
            return true;
        }
        //state is a MIN state
        if (color == -1 && !test(child, depth - 1, score, -color, condition)){
            return false;
        }
    }

    // The player can't make moves, so we check the other player's turn.
    if (nchildren == 0) {
        //state is a MAX state
        if (color == 1  && test(state, depth - 1, score, -color, condition)){
            return true;
        }
        //state is a MIN state
        if (color == -1 && !test(state, depth - 1, score, -color, condition)){
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
            score = scout(child, depth - 1, -color, false);
        } else {
            //State is a MAX state.
            if (color == 1 && test(child, depth - 1, score, -color, 0)){
                score = scout(child, depth - 1, -color, false);
            }
            //State is a MIN state.
            if (color == -1 && !test(child, depth - 1, score, -color, 1)){
                score = scout(child, depth - 1, -color, false);
            }
        }
    }

    //The player can't make moves, so we check the other player's turn
    if (nchildren == 0) {
      score = scout(state, depth - 1, -color, false);
    }

    return score;
}

int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false) {
	if (state.terminal()){
		return color * state.value();
	}

	int score;


    std::vector<state_t> children = get_children(state, color);

    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
    	if (i == 0){
    		score = -negascout(child, depth - 1, -beta, -alpha, -color)
    	}else{
    		score = -negascout(child, depth - 1, -alpha - 1, -alpha, -color)
    	}

		if ((alpha < score)&&(score < beta)){
			score = -negascout(child, depth - 1, -beta, -score, -color)
			alpha = max(alpha, score)

			if (alpha >= beta){
				break;
			}

		}
	}

	return alpha
}
