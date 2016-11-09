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
	if (state.terminal()){
		return state.value();
	}

	int  score = 0;
    bool black = depth % 2 == 0; // black moves first!
    std::vector<state_t> children = get_children(state, black);

    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
    	child = children[i];
    	if (i==0){
    		score = scout(child, depth - 1);
    	}else{
    		if (black && TEST(child,score,0)){
    			score=scout(child, depth - 1);
    		}
    		if (!black && !TEST(child,score,1)){
    			score=scout(child, depth - 1);
    		}
    	}
    }

	return score;
}

// Condition 0 = >, 1 = >=
bool TEST(state_t state, int depth, int score, int condition){
	if (state.terminal()){
		if (condition==0){
			return state.value() > score ? true : false;
		}else if(condition==1){
			return state.value() >= score ? true : false;	
		} 
		
	}

  	bool black = depth % 2 == 0; // black moves first!
    std::vector<state_t> children = get_children(state, black);

    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
    	child = children[i];
		if (black && TEST(child,depth - 1,0)){
			return true;
		}
		if (!black && !TEST(child,depth - 1,0)){
			return false;
		}
    }

	return black ? false : true;
	
}

int negascout(state_t state, int depth, int alpha, int beta, int color)

	if (state.terminal()){
		//h := heuristic(node)
		return color * state.value();
	}

	int score;
	
	bool black = depth % 2 == 0; // black moves first!
    std::vector<state_t> children = get_children(state, black);

    int nchildren = children.size();
    state_t child;

    for (int i = 0; i < nchildren; ++i) {
    	if (i==0){
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