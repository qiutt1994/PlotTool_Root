#include <iostream>
#include <string>
#include <TH1.h>
#include <TFile.h>
#include "region.h"
using namespace std;

int main()
{
    std::vector<std::string> sample_list = {)
    sample_list = {"hist-ZmumuL_Sh221.root"};
    TFile *f1 = new TFile("hadd_2lep_mc16d_produced_by_gitlab-CI.root","OPEN");
    //TFile *o1 = (TFile *)f1->Get("Systematics;1");

    for(auto k : *o1->GetListOfKeys()) {
        TH1F *hist;
        //hist = (TH1F *) o1->Get(k->GetName());
        std::string s = k->GetName();
        std::string token = s.substr(0, s.find("_"));
    }
    cout << endl;
    return 0;
}