#include "Histogram.h"
#include "jsonlib.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

Histogram::Histogram()
{
	binning = {};
	content = {};
	statistics = {};
	systematics = {};
}

Histogram::Histogram(std::vector<double> bin, std::vector<double> con, std::vector<double> sta, std::vector<double> sys)
{
	if (bin.size()==0)
	{
		cout << "Histogram::Histogram: must have at least one bin." << endl;
		exit(1);
	}
	if (bin.size() - 1 == con.size() && con.size() == sta.size())
	{
		binning = bin;
		content = con;
		statistics = sta;
		systematics = {};
		if (sys.size() != 0)
		{
			if (sys.size() == con.size())
				systematics = sys;
			else
			{
				cout << "Error: invalid systematics bin size." << endl;
				exit(1);
			}
		}
	}
	else
	{
		cout << "Error: invalid bin size." << bin.size() - 1 << "  " << con.size() <<"  "<< sta.size() << endl;
		exit(1);
	}
}

// copy constructor
Histogram::Histogram(const Histogram& input)
{
	if (&input != this)
	{
		binning = input.binning;
		content = input.content;
		statistics = input.statistics;
		systematics = input.systematics;
		name = input.name;
	}
}

Histogram::~Histogram()
{
}
// get histogram size
int Histogram::size()
{
	return content.size();
}

// merge histogram
void Histogram::add(Histogram h1, string note)
{
	notes.push_back(note);
	// check size
	if (size() != h1.size())
	{
		cout << "Error: binnings do not match.";
		exit(1);
	}
	for (int i = 0; i < size(); i++)
	{
		content[i] += h1.content[i];
		double stat{ 0 };
		stat = sqrt(pow(statistics[i], 2) + pow(h1.statistics[i], 2));
		statistics[i] = stat;
		if (systematics.size() != 0 && h1.systematics.size() != 0)
		{
			stat = 0;
			stat = sqrt(pow(systematics[i], 2) + pow(h1.systematics[i], 2));
			systematics[i] = stat;
		}

	}
}

std::vector<double> Histogram::subtraction(Histogram input)
{
	std::vector<double> result;
	if (size() == input.size())
	{
		for (int i = 0; i < size(); i++)
		{
			result.push_back(input.content[i] - content[i]);
		}
	}
	else
	{
		cout << "Error: subtraction failed due to different bin size.";
		exit(1);
	}
	return result;
}

double Histogram::sum()
{
	double output{ 0. };
	for (int i{ 0 }; i < size(); i++)
	{
		output += content[i];
	}
	return output;
}


// print everything
void Histogram::print()
{
	cout << "\nbins: ";
	for (auto i: binning)
	{
		cout << i << " ";
	}
	cout << endl;

	cout << "content: ";
	for (auto i : content)
	{
		cout << i << " ";
	}
	cout << endl;

	cout << "stat: ";
	for (auto i : statistics)
	{
		cout << i << " ";
	}
	cout << endl;

	cout << "syst: ";
	for (auto i : systematics)
	{
		cout << i << " ";
	}
	cout << endl;
}

string Histogram::json()
{
	string result{ "{" };
	stringstream ss;
	ss << "\"binning\": " << vectertojson<double>(binning) << ", \"content\": " << vectertojson<double>(content)
		<< ", \"stat\": " << vectertojson<double>(statistics) << ", \"syst\": " << vectertojson<double>(systematics)
		<<  ", \"name\": "  << "\"" << name << "\""
		<<"}";
	result += ss.str();
	return result;
}
