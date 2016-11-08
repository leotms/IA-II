int maxmin(state_t state, int depth, bool use_tt) {
	if (depth == 0 || state.terminal()) {
		return h(node);
	}

	int score = MAXINT;
	foreach child of node {
		score := min(score, maximin(child, depth -1));
	}
}


int minmax(state_t state, int depth, bool use_tt = false) {
	if (depth == 0 || state.terminal()) {
		return h(node);
	}

	int score = -MAXINT;
	foreach child of node {
		score := max(score, minmax(child, depth -1));
	}
}

int negamax(state_t state, int depth, bool color)
{
	if (depth == 0 || state.terminal()){
		return color * h(node);
	}

	int alpha = MAXINT;
	foreach child of node {
		alpha = max(alpha, -negamax(child, depth - 1, !color));	
	}
	return alpha;
}

// NEGAMAX with ALPHA-BETA pruning

int negamax_ab(state_t state, int depth, int alpha, int beta, bool color){
	if (depth == 0 || state.terminal()){
		h = h(node);
		return color * h; 
	}

	int score = MAXINT;
	foreach child of node{
		val = -negamax_ab(child, depth-1, -beta, -alpha, !color){
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