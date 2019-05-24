#pragma once

#include<vector>
#include <iostream>
#include"Histogram.h"
using namespace std;

class region
{
private:
	std::vector<std::vector<Histogram*>> sys_updown;
	std::vector<Histogram*> sys_oneside;
	std::vector<string> sys_updown_name;
	std::vector<string> sys_oneside_name;
public:
	Histogram nominal;
	Histogram systematics;
	Histogram shape;
	vector<Histogram> individal_nominal;

	region();
	region(Histogram);
	region(Histogram, vector<Histogram>);
	~region();

    // get size
	int size();

	// add new systematics
	void add_sys(Histogram,string sys_name = "none");
	void add_sys(Histogram, Histogram,string sys_name = "none");
	// calculate systematics
	void calculate_sys();
	// convert object to json
	string json();
	string getsystable();
};
