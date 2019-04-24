#include <iostream>
#include <string>
#include <TH1.h>
#include <TFile.h>
#include "region.h"
#include "Histogram.h"
#include <sstream>
#include <typeinfo>
using namespace std;

struct branch_type
{
	string sample;
	string tag;
	string region;
	string variable;
	string sys;
	string updown;
};
std::vector<std::string> sample_list = {"W", "Wl", "Wcl", "Wbl", "Wbb", "Wbc", "Wcc", "WZ", "WW", "Zcc", "Zcl", "Zbl", "Zbc", "Zl", "Zbb", "Z", "ZZ", "stopWt", "stops", "stopt", "ttbar"};
string fileaddress = "hadd_2lep_mc16d_produced_by_gitlab-CI.root";

// splite string
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

branch_type get_branch_type(string input)
{
	branch_type output;
	vector<string> sub = split(input,'_');
	output.sample = sub[0];
	output.tag = sub[1];
	output.region = sub[3];
	output.updown = sub[sub.size() - 1];

	int sysstart{int(sub.size())};
	for(int i{0}; i< sub.size(); i++)
	{
		if (sub[i].size() < 3) continue;
		if(sub[i].substr(0,3) == "Sys")
		{
			sysstart = i;
			break;
		}
	}
	for (int i{4}; i < sysstart; i++)
	{
		output.variable += sub[i];
		if(i != sysstart-1)
			output.variable += "_";
  }
		for (int i{sysstart}; i < sub.size()-1; i++)
		{
			output.sys += sub[i];
			if(i != sub.size()-1)
				output.sys += "_";
		}
	return output;
}

std::vector<string> discover_sys()
{
	TFile *f1 = new TFile(fileaddress.c_str(),"OPEN");
	TFile *o1 = (TFile *)f1->Get("Systematics;1");
	vector<string> output;
	for(auto k : *o1->GetListOfKeys())
	{
		vector<string> sub = split(k->GetName(),'_');
		string sysname;
		if (sub.size() < 3) continue;
		branch_type subs = get_branch_type(k->GetName());
		sysname = subs.sys;
		if(std::find(output.begin(), output.end(), sysname) == output.end())
			output.push_back(sysname);
			//cout << k->GetName()<<"     "<<sysname <<endl;}
	}
	//for (string each: output) cout<<each<<" ";
	return output;
}
// creat historam object using root TH1F
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


// create final histogram can calculate systematics
region create_hist(std::vector<string> tags, string theregion, string varible, bool sys = false)
{
	Histogram nominal;
	std::vector<Histogram> sub_nominal;
	TFile *f1 = new TFile(fileaddress.c_str(),"OPEN");
	// locate and stack nominal tree
	for(auto k : *f1->GetListOfKeys())
	{
			vector<string> sub = split(k->GetName(),'_');
			if (sub.size() < 3)
				continue;
			branch_type subs = get_branch_type(k->GetName());
			if(std::find(sample_list.begin(), sample_list.end(), subs.sample) == sample_list.end())
				continue;
			if(std::find(tags.begin(), tags.end(), subs.tag) == tags.end())
				continue;
			if(subs.region != theregion)
				continue;
			if(subs.variable != varible)
			{
				continue;
			}
			TH1F *hist;
			hist = (TH1F *) f1->Get(k->GetName());

			if(nominal.size()==0)
			 nominal = loadhist(hist);
			else
				nominal.add(loadhist(hist));
			// add subnomial

			bool exist = false;
			for (int i{0}; i < sub_nominal.size(); i++)
			{
				if (sub_nominal[i].name == subs.sample)
				{
					sub_nominal[i].add(loadhist(hist));
					exist = true;
					break;
				}
			}
			if(!exist)
			{
				Histogram tem  = loadhist(hist);
				tem.name = subs.sample;
				sub_nominal.push_back(tem);
			}
	}

	region output(nominal, sub_nominal);
	if(sys)
	{
		// discover systematics
		vector<string> sys_up;
		vector<string> sys_down;
		vector<vector<string>> sys_updown;
		vector<string> sys_oneside;
		TFile *o1 = (TFile *)f1->Get("Systematics;1");
		for(auto k : *o1->GetListOfKeys())
		{
			string branch_name = k->GetName();
      vector<string> sub = split(k->GetName(),'_');
			if (sub.size() < 3)
				continue;
			branch_type subs = get_branch_type(k->GetName());
			if(std::find(sample_list.begin(), sample_list.end(), sub[0]) == sample_list.end())
				continue;
			if(std::find(tags.begin(), tags.end(), sub[1]) == tags.end())
				continue;
			if(subs.region != theregion)
				continue;
			if(subs.variable != varible)
				continue;
			if(subs.updown == "1up")
			  sys_up.push_back(branch_name.substr(0,branch_name.size()-3));
			else if(subs.updown == "1down")
				sys_down.push_back(branch_name.substr(0,branch_name.size()-5));
			else
				sys_oneside.push_back(branch_name);
    }
		for(int i = 0; i < sys_up.size(); i++)
		{
			if(std::find(sys_down.begin(), sys_down.end(), sys_up[i]) != sys_down.end())
			{
				vector<string> tem = {sys_up[i] + "1up", sys_up[i] + "1down"};
				sys_updown.push_back(tem);
				sys_down.erase(std::find(sys_down.begin(), sys_down.end(), sys_up[i]));
			}
			else
			{
				sys_oneside.push_back(sys_up[i] + "1up");
			}
		}
		if(sys_down.size()>0)
			cout << "Warning: unmatched sys_down.";

		// add systematics to histogram
		cout << "here"<<endl;
		for(auto i_updown: sys_updown)
		{
			TH1F *hist1 = (TH1F *) o1->Get(i_updown[0].c_str());
			TH1F *hist2 = (TH1F *) o1->Get(i_updown[1].c_str());
			Histogram up = loadhist(hist1);
			Histogram down = loadhist(hist2);
			output.add_sys(up,down);
		}
		cout << "here"<<endl;
		for (auto i_oneside: sys_oneside)
		{
			TH1F *hist1 = (TH1F *) o1->Get(i_oneside.c_str());
			Histogram oneside = loadhist(hist1);
			output.add_sys(oneside);
		}
		cout << "here"<<endl;
  }
	//output.calculate_sys();
	cout<<output.json()<<endl;
	return output;
}

int main()
{

	std::vector<string> tags = {"0tag2pjet", "1tag2pjet"};
	string theregion = "SR";
	string varible = "pTBB";
	create_hist(tags,theregion,varible,true);
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
