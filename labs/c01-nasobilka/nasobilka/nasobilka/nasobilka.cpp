#include <print>
#include <vector>
#include <string>
using namespace std;

class Nasobitel {
private:
	int from;
	int to;
public:
	Nasobitel(int odkud, int kam) {
		from = odkud;
		to = kam;
	}
	void udelej(int cislo);
};

void Nasobitel::udelej(const int cislo) {
	for (int i = from; i <= to; i++) {
		println("{} * {} = {}", i, cislo, i * cislo);
	}
}

int main(int argc, char ** argv)
{
	vector<string> arg(argv, argv + argc);
	if (arg.size() > 1) {
		int argstart = 1;
		int from = 1;
		if (arg[1] == "-f") {
			from = stoi(arg[2]);
			argstart+=2;
		}
		int to = 10;
		if (arg[3] == "-t") {
			to = stoi(arg[4]);
			argstart+=2;
		}

		Nasobitel alice(from, to);
		for (int i = argstart; i < arg.size(); i++) {
			int nmb = stoi(arg[i]);
			alice.udelej(nmb);
		}
	}
}


