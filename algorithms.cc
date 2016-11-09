int minmax(state_t state, int depth, bool use_tt = false) {
    if (state.terminal()) {
        return state.value();
    }

    int score = INT_MIN;
    bool player = depth % 2 == 0; // black moves first!
    std::vector<state_t> children = get_children(state, player);

    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        score = min(score, maxmin(child, depth - 1, false));
    }
    return score;
}

int maxmin(state_t state, int depth, bool use_tt = false) {
    if (state.terminal()) {
        return state.value();
    }

    int  score = INT_MAX;
    bool player = depth % 2 == 0; // black moves first!
    std::vector<state_t> children = get_children(state, player);

    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
        child = children[i];
        score = max(score, minmax(child, depth - 1));
    }
    return score;
}

int negamax(state_t state, int depth, int color)
{
	if (depth == 0 || state.terminal()){
		return color * h(node);
	}

	int alpha = MAXINT;
	foreach child of node {
		alpha = max(alpha, -negamax(child, depth - 1, -color));	
	}
	return alpha;
}

// NEGAMAX with ALPHA-BETA pruning

int negamax_ab(state_t state, int depth, int alpha, int beta, int color){
	if (depth == 0 || state.terminal()){
		h = h(node);
		return color * h; 
	}

	int score = MAXINT;
	foreach child of node{
		val = -negamax_ab(child, depth-1, -beta, -alpha, -color){
			score = max(score, val);
			alpha = max(alpha, val);
			if(alpha >= beta){
				break;
			}
		}
	}
	return score;

}

int scout(state_t state, int depth){
	if (depth == 0 || state.terminal()){
		return h(node);
	}

	int score = MAXINT;
	foreach child of node {
		if (child is first child){
			score = scout(child, depth - 1);
		}
		else{
			if(node is Max && TEST(child, score, >)){
				score := scout(child, depth - 1);
			}
			if(node is Min && !TEST(child, score, >=)){
				score := scout(child, depth - 1);
			}
		}
	}

	return score;
}

TEST(state_t state, int depth, int score, Condition >){
	if (depth == 0 || state.terminal()){
		return h(node) > score ? true : false;
		

		foreach child of node{
			if( node is Max && TEST(child, depth - 1, score, >)){
				return true;
			}
			if(node is Min && !TEST(child, depth - 1, score, >)){
				return false;
			}
		}

		return node is Max ? false : true;
	}
}