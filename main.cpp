#include <iostream>
#include <string>
#include <TH1.h>
#include <TFile.h>
#include "region.h"
#include "Histogram.h"
#include <sstream>
#include <typeinfo>
#include <fstream>
using namespace std;

struct branch_type
{
	string full;
	string nominalname;
	string sample;
	string tag;
	string region;
	string variable;
	string sys;
	string updown;
};
std::vector<std::string> sample_list = {"Wl", "Wcl", "Wbl", "Wbb", "Wbc", "Wcc", "WZ", "WW", "Zcc", "Zcl", "Zbl", "Zbc", "Zl", "Zbb", "ZZ", "stopWt", "stops", "stopt", "ttbar", "ggZqqZll_Sh222", "ggWqqWlv_Sh222" "ggZllH125", "qqZllH125"};
//std::vector<std::string> sample_list = {"WZ"};
string fileaddress = "oldpaper.root";
string period = "a";
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
	output.full = input;
	vector<string> sub = split(input,'_');
	int theindex{0};
	output.sample = sub[theindex++];
	if(!(sub[theindex].at(0) =='0' || sub[theindex].at(0) =='1' || sub[theindex].at(0) =='2' || sub[theindex].at(0) =='3'))
	{
		output.sample += "_" + sub[theindex++];
	}
	output.tag = sub[theindex++];
	theindex++;
	output.region = sub[theindex++];
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
	for (int i{theindex++}; i < sysstart; i++)
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
	output.nominalname = input.substr(0, input.find("Sys"));
	return output;
}

/*
// conservative version
branch_type get_branch_type(string input)
{
	branch_type output;
	output.full = input;
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
	output.nominalname = input.substr(0, input.find("Sys"));
	return output;
}*/

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
		
		if(std::find(output.begin(), output.end(), sysname) == output.end()){
			output.push_back(sysname);
			cout << sysname << endl;
		}
			//cout << k->GetName()<<"     "<<sysname <<endl;}
	}
	//for (string each: output) cout<<each<<" ";
	cout << "systematics discovered." <<endl;
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
string create_hist(std::vector<string> tags, string theregion, string varible, bool sys = false)
{
	Histogram nominal;
	std::vector<Histogram> sub_nominal;
	std::vector<branch_type> nominaltype;
	TFile *f1 = new TFile(fileaddress.c_str(),"OPEN");
	cout << "Loading nominal" << endl;
	// locate and stack nominal tree
	for(auto k : *f1->GetListOfKeys())
	{
			vector<string> sub = split(k->GetName(),'_');
			if (sub.size() < 4)
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
			nominaltype.push_back(subs);
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

	//for (auto each: sub_nominal){
//		cout << each.name<<endl;
	//}

	region output(nominal, sub_nominal);

	if(sys)
	{
		cout << "loading systematics" << endl;
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
			cout << "Warning: unmatched sys_down " << sys_down.size() << endl;

//----------------------------------------------------------------------------------------------------------------
		// add systematics to histogram
		std::vector<string> allsysname = discover_sys();
		std::vector<string> allsysnameupdown;
		std::vector<string> allsysnameonside;
		vector<vector<branch_type>> updowntype;
		vector<branch_type> onesidetype;
		for(auto i_updown: sys_updown)
			updowntype.push_back({get_branch_type(i_updown[0]), get_branch_type(i_updown[1])});
		for (auto i_oneside: sys_oneside)
			onesidetype.push_back(get_branch_type(i_oneside));
		for(auto each_sys: allsysname)
		{
			bool foundit = false;
			int j = 0;
			for(int i{0}; i< updowntype.size(); i++)
			{
				if (updowntype[i][0].sys == each_sys)
				{
					foundit = true;
					break;
				}
			}
			if(foundit)
				allsysnameupdown.push_back(each_sys);
			else
				allsysnameonside.push_back(each_sys);
		}
		// open nominal tree
		TFile *no = new TFile(fileaddress.c_str(),"OPEN");
		//try to find all systematics in sys tree first. then go to nominaltype.
		for(auto each: allsysnameupdown)
		{
			Histogram totalhistup;
			Histogram totalhistdown;
			std::vector<string> sampleused;
			for(auto each_address: updowntype)
			{
				if(each == each_address[0].sys)
				{
					sampleused.push_back(each_address[0].sample);
					TH1F *hist1 = (TH1F *) o1->Get(each_address[0].full.c_str());
					TH1F *hist2 = (TH1F *) o1->Get(each_address[1].full.c_str());
					Histogram up = loadhist(hist1);
					Histogram down = loadhist(hist2);
					if (totalhistup.size() > 0)
					{
						totalhistup.add(up);
						totalhistdown.add(down);
						continue;
					}
					totalhistup = up;
					totalhistdown = down;
				}
			}
			// here find sample not used
			for(auto each_sample: sample_list)
			{
				if(std::find(sampleused.begin(), sampleused.end(), each_sample) == sampleused.end())
				{
					for(auto each_no: nominaltype)
					{
						if(each_no.sample == each_sample)
						{
							TH1F *hist1 = (TH1F *) no->Get(each_no.full.c_str());
							Histogram up = loadhist(hist1);
							if (totalhistup.size() > 0)
							{
								totalhistup.add(up);
								totalhistdown.add(up);
								continue;
							}
							totalhistup = up;
							totalhistdown = up;
						}
					}
				}
			}
			output.add_sys(totalhistup,totalhistdown);
		}
		// one side loop here
		for(auto each: allsysnameonside)
		{
			Histogram totalhistup;
			std::vector<string> sampleused;
			for(auto each_address: onesidetype)
			{
				if(each == each_address.sys)
				{
					sampleused.push_back(each_address.sample);
					TH1F *hist1 = (TH1F *) o1->Get(each_address.full.c_str());
					Histogram up = loadhist(hist1);
					if (totalhistup.size() > 0)
					{
						totalhistup.add(up);
						continue;
					}
					totalhistup = up;
				}
			}
			// here find sample not used
			for(auto each_sample: sample_list)
			{
				if(std::find(sampleused.begin(), sampleused.end(), each_sample) == sampleused.end())
				{
					for(auto each_no: nominaltype)
					{
						if(each_no.sample == each_sample)
						{
							TH1F *hist1 = (TH1F *) no->Get(each_no.full.c_str());
							Histogram up = loadhist(hist1);
							if (totalhistup.size() > 0)
							{
								totalhistup.add(up);
								continue;
							}
							totalhistup = up;
						}
					}
				}
			}
			output.add_sys(totalhistup);
		}

		// claculate systematics
		cout << "calculating systematics" << endl;
		output.calculate_sys();
  }
	return output.json();
	//return output;
}

void make_plot(std::vector<string> tags, string theregion, string variable)
{

	string filename;
	filename = period + "_" + variable + "_" + theregion + "_";
	for (auto each: tags)
		filename += each +"-";
	filename += "_.json";
	std::vector<std::string> sample_list_backup = sample_list;
	string mc = create_hist(tags,theregion,variable,true);
	sample_list = {"data"};
	string data = create_hist(tags,theregion,variable,false);
	ofstream myfile;
	myfile.open ("jsonoutput/" + filename);
	myfile << mc <<"\n";
	myfile << data <<"\n";
	myfile.close();
	sample_list = sample_list_backup;
}

int main()
{
	fileaddress = "sample/combined.root";
	std::vector<string> tags = {"2tag2pjet"};
	string theregion = "mBBcr";
	//string theregion = "SR";
	//string theregion = "topemucr";
	string variable = "mVH";
	period = "run2";
	make_plot(tags, theregion, variable);
	/*string mc = create_hist(tags,theregion,varible,true);
	ofstream myfile;
	myfile.open ("mVHsr.txt");
	myfile << mc <<"\n";
	myfile.close();

	sample_list = {"data"};
	string test2 = create_hist(tags,theregion,varible,false);
	ofstream myfile1;
	myfile1.open ("mVHsrdata.txt");
	myfile1 << test2 <<"\n";
	myfile1.close();
*/

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
