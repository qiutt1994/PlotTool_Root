#include "region.h"
#include <random>
#include<cstdlib>
#include "Expdata.h"
#include <iostream>
#include <sstream>
#include <string>

region::region()
{
}

region::region(Histogram input) :
	nominal{ input }
{
	nominal.name = "sum";
};

region::region(Histogram input, vector<Histogram> consist) :
	nominal{ input }
	{
		nominal.name = "sum";
		for(auto i: consist)
		{
			if(nominal.size() == i.size())
				individal_nominal.push_back(i);
			else
			{
				cout << "Error: cannot add sub histogram.";
				exit(1);
			}
	  }
	};

region::~region()
{
	for (int i = 0; i < int(sys_updown.size()); i++)
	{
		delete sys_updown[i][0];
		delete sys_updown[i][1];
	}
	for (int i = 0; i < int(sys_oneside.size()); i++)
	{
		delete sys_oneside[i];
	}
}

// get size
int region::size()
{
	return nominal.size();
}

// add new oneside systematics
void region::add_sys(Histogram input)
{
	if (size() == input.size())
	{
		Histogram* sys = new Histogram(input);
		sys_oneside.push_back(sys);
	}
	else
	{
		cout << "Error: add_sys bin size do not match.";
		exit(1);
	}
}

// add new twoside systematics
void region::add_sys(Histogram input1, Histogram input2)
{
	if (size() == input1.size() && size() == input2.size())
	{
		std::vector<Histogram*> total;
		total.push_back(new Histogram(input1));
		total.push_back(new Histogram(input2));
		sys_updown.push_back(total);
	}
	else
	{
		cout << "Error: add_sys bin size do not match.";
		exit(1);
	}
}

// calculate systematics
void region::calculate_sys()
{
	int number_of_exp =10000;

	// normal distribution generator
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0, 1);

    // every experiment in each bin
	vector<Expdata*> all_exp;
	for (int i_bin = 0; i_bin < size(); i_bin++)
		all_exp.push_back(new Expdata());
	// each experiment
	for(int i = 0; i < number_of_exp; i++)
	{
		std::vector<double> total_diff(size(), 0);
		// each systematics with up and down
		for(auto each_sysupdown: sys_updown)
		{
			double number = distribution(generator);
			std::vector<double> diff;
			if (number >= 0)
			{
				diff = nominal.subtraction(*each_sysupdown[0]);
			}
			if (number < 0)
			{
				diff = nominal.subtraction(*each_sysupdown[1]);
			}
			for (int i_bin = 0; i_bin < size(); i_bin++)
				total_diff[i_bin] += abs(number) * diff[i_bin];
		}
		// each systematics onesside
		for (auto each_sysoneside : sys_oneside)
		{
			double number = distribution(generator);
			std::vector<double> diff = nominal.subtraction(*each_sysoneside);
			for (int i_bin = 0; i_bin < size(); i_bin++)
				total_diff[i_bin] += number * diff[i_bin];
		}
		for (int i_bin = 0; i_bin < size(); i_bin++)
		{
			all_exp[i_bin]->append(total_diff[i_bin] + nominal.content[i_bin]);
		}
	}
	nominal.systematics = vector<double>(size(), 0.);
	vector<double> newheight(size(), 0.);
	//vector<double> newstat(size(), 0.);
	for (int i_bin = 0; i_bin < size(); i_bin++)
	{
		nominal.systematics[i_bin] = pow(all_exp[i_bin]->sigma2(), 0.5);
		newheight[i_bin] = all_exp[i_bin]->mean();
	}
	systematics = Histogram(nominal.binning, newheight, nominal.statistics, nominal.systematics);
	vector<double> shape_sys(size(), 0.);
	vector<double> shape_height(size(), 0.);
	for (int i_bin = 0; i_bin < size(); i_bin++)
	{
		all_exp[i_bin]->rescale(nominal.sum()/systematics.sum());
		shape_sys[i_bin] = pow(all_exp[i_bin]->sigma2(), 0.5);
		shape_height[i_bin] = all_exp[i_bin]->mean();
	}
	shape = Histogram(nominal.binning, shape_height, nominal.statistics, shape_sys);
	// free memory

	for (int i_bin = 0; i_bin < size(); i_bin++)
		delete all_exp[i_bin];
}
// convert object to json
// unfinished
string region::json()
{
	string output{"{\"nominal\": "};
	output += nominal.json() + ", ";
	output += "\"shape\": ";
	output += shape.json() + ", ";
	for( auto each: individal_nominal)
	{
		output += "\"" + each.name + "\": ";
		output += each.json() + ", ";
	}
	output = output.substr(0, output.size()-2);
	output += "}";
	return output;
}
