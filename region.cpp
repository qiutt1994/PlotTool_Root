#include "region.h"
#include <random>
#include<cstdlib>
#include "Expdata.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

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
void region::add_sys(Histogram input, string sys_name)
{
	if (size() == input.size())
	{
		Histogram* sys = new Histogram(input);
		sys_oneside.push_back(sys);
		sys_oneside_name.push_back(sys_name);
	}
	else
	{
		cout << "Error: add_sys bin size do not match.";
		exit(1);
	}
}

// add new twoside systematics
void region::add_sys(Histogram input1, Histogram input2, string sys_name)
{
	if (size() == input1.size() && size() == input2.size())
	{
		std::vector<Histogram*> total;
		total.push_back(new Histogram(input1));
		total.push_back(new Histogram(input2));
		sys_updown.push_back(total);
		sys_updown_name.push_back(sys_name);
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
		//----------
		// std::vector<double> syssum(size(), 0);
		// int countsys = 0;
		//---------
		std::vector<double> total_diff(size(), 0);
		// each systematics with up and down
		for(auto each_sysupdown: sys_updown)
		{
			double number = distribution(generator);

			//-------
			//countsys +=2;
			// std::vector<double> diffup;
			// std::vector<double> diffdo;
			// diffup = nominal.subtraction(*each_sysupdown[0]);
			// diffdo = nominal.subtraction(*each_sysupdown[1]);
			// for (int i_bin = 0; i_bin < size(); i_bin++)
			// 	syssum[i_bin] += diffup[i_bin]*diffup[i_bin] + diffdo[i_bin]*diffdo[i_bin];
			//-------

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
			
			//-------
			// countsys +=1;
			// std::vector<double> diffup;
			// diffup = nominal.subtraction(*each_sysoneside);

			// for (int i_bin = 0; i_bin < size(); i_bin++){
			// 	syssum[i_bin] += diffup[i_bin]*diffup[i_bin]/2 + diffup[i_bin]*diffup[i_bin]/2;
			// 	cout << each_sysoneside->content[i_bin] <<"------";}
			// cout << endl;
			//-------

			double number = distribution(generator);
			std::vector<double> diff = nominal.subtraction(*each_sysoneside);
			for (int i_bin = 0; i_bin < size(); i_bin++)
				total_diff[i_bin] += number * diff[i_bin];
		}
		for (int i_bin = 0; i_bin < size(); i_bin++)
		{
			//--------
			//cout << syssum[i_bin] << "    ";
			//--------
			double each_all_exp = total_diff[i_bin] + nominal.content[i_bin];
			if( each_all_exp <0 ) 
			{
				//cout << each_all_exp << "   ";
				each_all_exp = 0;
			}
			all_exp[i_bin]->append(each_all_exp);
		}
		//-----
		// cout <<"\n"<<countsys<<endl;
		// exit(1);
		//-----
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

string region::getsystable()
{
	string output = "";
	vector<string> allsysname;
	for(int i = 0;i<sys_oneside.size();i++)
	{
		double diff = 0;
		double diff_shape = 0;
		diff = (sys_oneside[i]->sum() - nominal.sum())/nominal.sum() * 100;
		diff_shape = nominal.binneddiff(*(sys_oneside[i]));
		output += sys_oneside_name[i] + " " + to_string(diff) + " " + to_string(diff_shape) + "\n";
		allsysname.push_back(sys_oneside_name[i].substr(0, sys_oneside_name[i].size()-2));
	}
	for(int i = 0;i<sys_updown.size();i++)
	{
		double diff = 0;
		double diff_shape = 0;
		diff_shape = nominal.binneddiff(*(sys_updown[i][0]));
		diff = (sys_updown[i][0]->sum() - nominal.sum())/nominal.sum() * 100;
		output += sys_updown_name[i] + " up " + to_string(diff)  + " " + to_string(diff_shape) + "\n";
		diff_shape = nominal.binneddiff(*(sys_updown[i][1]));
		diff = (sys_updown[i][1]->sum() - nominal.sum())/nominal.sum() * 100;
		output += sys_updown_name[i] + " down " + to_string(diff)  + " " + to_string(diff_shape) + "\n";
		allsysname.push_back(sys_updown_name[i].substr(0, sys_updown_name[i].size()-2));
	}
	ofstream myfile;
	myfile.open ("allsysname.txt");
	for(auto eachname:allsysname)
	{
		myfile<<"ShapeSyst = " << eachname << "\n";
	}
	myfile.close();
	return output;
}