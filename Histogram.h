#pragma once

#include <iostream>
#include <vector>
using namespace std;

class Histogram
{
public:
	std::vector<double> binning;
	std::vector<double> content;
	std::vector<double> statistics;
	std::vector<double> systematics;
	string name = "noname";
	// Default constructor
	Histogram();
	// Parameterized constructor
	Histogram(std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double> = {});
	// Copy constructor
	Histogram(const Histogram&);

	// Destructor
	~Histogram();
	// get histogram size
	int size();
	// merge histogram
	void add(Histogram);
	// retrun difference between bins
	std::vector<double> subtraction(Histogram);
	// return total number of event
	double sum();
	// print everything
	void print();
	string json();
};
