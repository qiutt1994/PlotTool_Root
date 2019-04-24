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
	void add_sys(Histogram);
	void add_sys(Histogram, Histogram);
	// calculate systematics
	void calculate_sys();
	// convert object to json
	string json();
};
