#pragma once
#include "../Populations.h"
#include "MyV.h"
#include "RingBuffer.h"
#include "ObjectGroup.h"
#include "../CustomShapes/ColumnShape.h"
#include "../CustomShapes/Plotter.h"

void calcAllFlows(human_pop& elite, human_pop& commoner, nature_stock& nat, wealth_stock& wealth) {
	calcFlow(elite);
	calcFlow(commoner);
	calcFlow(nat, commoner);
	calcFlow(wealth, nat, elite, commoner);
}

lalg::vec4 calcAllStocks(human_pop& elite, human_pop& commoner, nature_stock& nat, wealth_stock& wealth) {
	return {
			calcStock(elite, 0, 1, true).stock,
			calcStock(commoner, 0, 1, true).stock,
			calcStock(nat, 0, 1, true).stock,
			calcStock(wealth, 0, 1, true).stock
	};
}
