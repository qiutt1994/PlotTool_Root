#pragma once
#include<vector>
#include<iostream>
using namespace std;

class Expdata
{
private:
	vector<double> data;
public:
	Expdata();
	Expdata(vector<double>);
	Expdata(const Expdata& input);
	~Expdata();
	int size() const;
	Expdata operator+(const Expdata&) const;
	double get_data(int);
	void append(double);
	void rescale(double);
	double mean();
	double sigma2();
	double sum();
};

