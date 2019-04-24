#include <iostream>
#include <string>
#include <TH1.h>
#include <TFile.h>
#include "region.h"
#include "Histogram.h"
#include <sstream>
using namespace std;

std::vector<std::string> sample_list = {"W", "Wl", "Wcl", "Wbl", "Wbb", "Wbc", "Wcc", "WZ", "WW", "Zcc", "Zcl", "Zbl", "Zbc", "Zl", "Zbb", "Z", "ZZ", "stopWt", "stops", "stopt", "ttbar"};
string fileaddress = "hadd_2lep_mc16d_produced_by_gitlab-CI.root";
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

Histogram loadhist(TH1F* input)
{
	vector<double> binning;
	vector<double> stat;
	vector<double> content;
	for(int i{1}; i <= input->GetNbinsX(); i++ )
	{
		binning.push_back(input->GetBinLowEdge(i));
		content.push_back(input->GetBinContent(i));
		stat.push_back(input->GetBinError(i));
	}
	binning.push_back(input->GetBinLowEdge(input->GetNbinsX()+1));
	return Histogram(binning,content,stat);
}
region creat_hist(std::vector<string> tags, string theregion, string varible, bool sys = false)
{
	Histogram nominal;
	TFile *f1 = new TFile("hadd_2lep_mc16d_produced_by_gitlab-CI.root","OPEN");
	for(auto k : *f1->GetListOfKeys())
	{
			//TH1F *hist;
			//hist = (TH1F *) o1->Get(k->GetName());
			vector<string> sub = split(k->GetName(),'_');
			if (sub.size() < 3)
				continue;
			if(std::find(sample_list.begin(), sample_list.end(), sub[0]) == sample_list.end())
				continue;
			if(std::find(tags.begin(), tags.end(), sub[1]) == tags.end())
				continue;
			if(sub[3] != theregion)
				continue;
			if(sub[4] != varible)
				continue;
			//cout << k->GetName() <<" ";
			TH1F *hist;
			hist = (TH1F *) f1->Get(k->GetName());
			if(nominal.size()==0)
			 nominal = loadhist(hist);
			else
				nominal.add(loadhist(hist));
	}
	region output(nominal);
	return output;
}

int main()
{

	std::vector<string> tags = {"0tag2pjet", "1tag2pjet"};
	string theregion = "SR";
	string varible = "pTBB";
	creat_hist(tags,theregion,varible);
    //std::vector<std::string> sample_list = {"W", "Wl", "Wcl", "Wbl", "Wbb", "Wbc", "Wcc", "WZ", "WW", "Zcc", "Zcl", "Zbl", "Zbc", "Zl", "Zbb", "Z", "ZZ", "stopWt", "stops", "stopt", "ttbar"};
    //sample_list = {"hist-ZmumuL_Sh221.root"};
    //TFile *f1 = new TFile("hadd_2lep_mc16d_produced_by_gitlab-CI.root","OPEN");
    //TFile *o1 = (TFile *)f1->Get("Systematics;1");
		/*
		std::vector<std::string> vec = {};
    for(auto k : *f1->GetListOfKeys()) {
        TH1F *hist;
        //hist = (TH1F *) o1->Get(k->GetName());
        vector<string> sub = split(k->GetName(),'_');
				if(std::find(vec.begin(), vec.end(), sub[0]) == vec.end())
				{
					  vec.push_back(sub[0]);
						cout << k->GetName() << " ";
				}
    }
		for(auto i:vec){
			  cout <<"\""<<i<<"\" ";
		}
		*/
    cout << endl;
    return 0;
}
