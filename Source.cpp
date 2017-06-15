#include <iostream>
#include <fstream>
#include <sstream>
#include <random> 
#include <time.h>
#include <thread>
#include <chrono>
#include <string>
#include <ctime>
#include <iomanip>
#include <mutex>

using namespace std;

/* GLOBAL VARIABLES */
int balance = 0;  //cannot be negative

mutex Mymutex;

int random_range(const int & min, const int & max)  // random int generator in [min, max]
{ 
	static mt19937 generator(time(0)); 
	uniform_int_distribution<int> distribution(min, max); 
	return distribution(generator); 
}

void showTime ()  // function that displays the current time
{
	time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
	struct tm *ptm = new struct tm; 
	localtime_s(ptm, &tt);
	cout<<put_time(ptm,"%X")<<endl;
}

void withdraw (const int & thread_id, const int & minW, const int & maxW, const int & countW, string filename)
{
	for (int i = 0; i < countW; i++)  // loop for withdraw
	{
		ifstream input;   // generating input for reading the file
		input.open(filename.c_str());

		string line;
		getline(input, line);
		stringstream ss(line);

		string dummy;
		int BalanceFile;

		while (ss >> dummy >> BalanceFile)
			balance = BalanceFile;  // reading balance from file and equating to it

		int time;
		time = random_range (minW, maxW);
		this_thread::sleep_for(chrono::seconds(time));  // sleep for the withdraw thread for a random amount of time

		Mymutex.lock();  // locking mutex to single thread operation for a time

		if (balance < 1)  // balance must be at least 1 for withdrawal
		{
			cout<<"Withdrawal thread "<<thread_id<<" failed to withdraw 1 TL; balance is "<<balance<<", ";
			showTime();
		}

		else
		{
			balance = balance - 1;  // updating balance after withdrawal
			cout<<"Withdrawal thread "<<thread_id<<" successfully withdrawn 1 TL; balance is "<<balance<<", ";
			showTime();
		}
		Mymutex.unlock();  // unlocking mutex for other thread operations

		ofstream output;                  // creating the output file as the same name
		output.open (filename.c_str());

		output<<"Balance "<<balance<<endl;  // balance value in the output
		output.close();
	 	input.close();
	}
}

void deposite (const int & minD, const int & maxD, const int & countD, string filename)
{
	for (int i = 0; i < countD; i++)  // loop for deposite
	{
		ifstream input;   // generating input for reading the file
		input.open(filename.c_str());

		string line;
		getline(input, line);
		stringstream ss(line);

		string dummy;
		int BalanceFile;

		while (ss >> dummy >> BalanceFile)
			balance = BalanceFile;  // reading balance from file and equating to it

		int time;
		time = random_range (minD, maxD);
		this_thread::sleep_for(chrono::seconds(time));  // sleep for the deposition thread for a random amount of time

		Mymutex.lock();  // locking mutex to single thread operation for a time
		balance = balance + 1;  // updating balance after deposition
		cout<<"1 TL has been deposited: balance is "<<balance<<", ";
		showTime();
		Mymutex.unlock();  // unlocking mutex for other thread operations
		
		ofstream output;                  // creating the output file as the same name
		output.open (filename.c_str());

		output<<"Balance "<<balance<<endl;  // balance value in the output
		output.close();
	 	input.close();
	}
}


int main ()
{
	string filename;
	int minW, maxW, minD, maxD, countW, countD;


	cout<<"Please, enter file name in which balance will be stored:"<<endl;
	cin>>filename;

	ofstream output;                  // creating the output file
	output.open (filename.c_str());

	output<<"Balance "<<balance<<endl;  // balance value in the output
	output.close();

	cout<<"Please, enter min and max waiting times between withdrawal transactions per thread (in seconds):"<<endl;
	cin>>minW>>maxW;

	cout<<"Please, enter min and max waiting times between deposit transactions (in seconds):"<<endl;
	cin>>minD>>maxD;

	cout<<"Please, enter the total number of withdrawal transactions per thread:"<<endl;
	cin>>countW;

	cout<<"Please, enter the total number of deposit transactions:"<<endl;
	cin>>countD;

	cout<<"Simulation starts at ";
	showTime();

	thread thr0 (&deposite, minD, maxD, countD, filename);  // thread for deposition is generated

	thread thr1 (&withdraw, 1, minW, maxW, countW, filename);  // first thread for withdrawal is generated
	thread thr2 (&withdraw, 2, minW, maxW, countW, filename);  // second thread for withdrawal is generated
	thread thr3 (&withdraw, 3, minW, maxW, countW, filename);  // third thread for withdrawal is generated

	thr0.join();   // all threads are joined/closed

	thr1.join();
	thr2.join();
	thr3.join();

	cout<<"Simulation ends at ";
	showTime();

	cin.ignore();
	cin.get();
	return 0;

}
