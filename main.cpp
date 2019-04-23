#include <iostream>
#include <string>
#include <TH1.h>
#include <TFile.h>
#include "region.h"
#include <sstream>
using namespace std;

vector<string> split(string input, char splitor)
{
	vector<string> output;
	std::istringstream ss(input);
	std::string token;
	while (std::getline(ss, token, '_'))
	{
		output.push_back(token);
	}
	return output;
}

int main()
{
    std::vector<std::string> sample_list = {"W", "Wl", "Wcl", "Wbl", "Wbb", "Wbc", "Wcc", "WZ", "WW", "Zcc", "Zcl", "Zbl", "Zbc", "Zl", "Zbb", "Z", "ZZ", "stopWt", "stops", "stopt", "ttbar"};
    sample_list = {"hist-ZmumuL_Sh221.root"};
    TFile *f1 = new TFile("hadd_2lep_mc16d_produced_by_gitlab-CI.root","OPEN");
    //TFile *o1 = (TFile *)f1->Get("Systematics;1");

		std::vector<std::string> vec = {};
    for(auto k : *f1->GetListOfKeys()) {
        TH1F *hist;
        //hist = (TH1F *) o1->Get(k->GetName());
        vector<string> sub = split(k->GetName(),'_');
				if(std::find(vec.begin(), vec.end(), sub[0]) == vec.end())
				{
					  vec.push_back(sub[0]);
				}
    }
		for(auto i:vec){
			  cout <<"\""<<i<<"\" ";
		}
    cout << endl;
    return 0;
}
