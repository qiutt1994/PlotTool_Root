#include "Expdata.h"
#include <cmath>


Expdata::Expdata()
{
}

Expdata::Expdata(vector<double> input)
{
	data = input;
}

Expdata::Expdata(const Expdata& input)
{
	if (&input != this)
	{
		data = input.data;
	}
}


Expdata::~Expdata()
{
}

int Expdata::size() const
{
	return data.size();
}

Expdata Expdata::operator+(const Expdata& input) const
{
	Expdata output(this->data);
	if (size() != input.size())
	{
		cout << "Error: Expdata::operator+ data size does not match.";
		exit(1);
	}
	for (int i = 0; i < size(); i++)
	{
		output.data[i] += input.data[i];
	}	
	return output;
}

double Expdata::get_data(int i)
{
	return data[i];
}

void Expdata::append(double input)
{
	data.push_back(input);
}

void Expdata::rescale(double input)
{
	for (int i = 0; i < size(); i++)
		data[i] *= input;
}

double Expdata::mean()
{
	double result{ 0 };
	for (int i = 0; i < size(); i++)
		result += data[i];
	result /= size();
	return result;
}

double Expdata::sigma2()
{
	double result{ 0 };
	double meanv(mean());
	for (int i = 0; i < size(); i++)
		result += pow(data[i] - meanv,2);
	result = result / (size() - 1.);
	return result;
}

double Expdata::sum()
{
	double output{ 0. };
	for (int i{ 0 }; i < size(); i++)
	{
		output += data[i];
	}
	return output;
}
