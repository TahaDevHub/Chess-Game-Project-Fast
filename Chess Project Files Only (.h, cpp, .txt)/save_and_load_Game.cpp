#include "save_and_load_Game.h"

//struct saveMoves {
//public:
//	static int count;
//	int tempSr, tempSc, tempDr, tempDc;
//};
void saveMove(ofstream& fout, int sr, int sc, int dr, int dc, string turn)
{

	fout << sr << " " << sc << " " << dr << " " << dc << endl;

}

void saveLoadedMove(ofstream& fout, int sr, int sc, int dr, int dc, string turn)
{

	fout << sr << " " << sc << " " << dr << " " << dc << endl;

}
