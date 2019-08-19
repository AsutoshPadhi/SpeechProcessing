#include<iostream>
#include<fstream>
#include<vector>
#include <bits/stdc++.h>

using namespace std;

long double shortTermEnergy(long long int frame[100])
{
    long long int sum = 0;
    for(int i=0; i<100; i++)
    {
        sum += frame[i]*frame[i];
    }

    return sum/100;
}

int zeroLevelCrossings(long long int frame[100])    /* Calculates the ZCR */
{
    int cnt=0;
    for(int i=1; i<100; i++)
    {
        if((frame[i]>=0 && frame[i-1]<0) || (frame[i]<0 && frame[i-1]>=0))
        {
            cnt++;
        }
    }

    return cnt;
}

void train(string filename)
{
    ifstream fin;
    ofstream fout;
    ofstream out;
    string line;

    fin.open("./Data/input/"+filename+".txt");
    fout.open("./Data/raw/"+filename+".csv");
    out.open("./Data/normalised/"+filename+".csv");

    
    int i=0;
    long long int frame[100];
    int frame_cnt=0;
    
    /* Get the entire data into a vector */
    vector<int> v;
    while(getline(fin, line))
    {
        if(i<5)
        {
            // nop
        }
        else
        {
            v.push_back(stoi(line));
        }
        i++;
    }
    
    /* Data normalisation */
    long long int max = *max_element(v.begin(), v.end());

    /* DC Shift */
    long double dc_avg=0;
    for(size_t i=0; i<100; i++)
    {
        dc_avg += v[i];
    }
    dc_avg = dc_avg/100;
    cout<<"DC Shift ("<<filename<<") = "<<dc_avg<<endl;
    for(size_t i=0; i<v.size(); i++)
    {
        v[i] -= dc_avg;
    }

    /* Normalize to bring the values in the range of 5000 to -5000 */
    for(size_t i=0; i<v.size(); i++)
    {
        v[i] = (v[i]*5000)/max;
    }

    vector<long double> s;
    vector<long double> z;
    vector<long double> window(10);
    
    /* For each frame, find STE and ZCR */
    int j=0;
    for(size_t i=0; i<v.size(); i++)
    {
        frame[j] = v[i];
        j++;
        if(j==100)
        {
            j=0;
            frame_cnt++;

            long double ste = shortTermEnergy(frame);
            int zcr = zeroLevelCrossings(frame);

            s.push_back(ste);
            z.push_back(zcr);

            fout<<frame_cnt<<","<<ste<<","<<zcr<<endl;
        }
    }
    
    /* Filtering out Noise */
    long double ste_avg;
    int frame_start;
    int frame_end;
    for(size_t i=0; i<s.size(); i++)
    {
        if(i<10)
        {
            window.push_back(s[i]);
            ste_avg = accumulate(window.begin(), window.end(), 0.0)/window.size();
        }
        else
        {
            if(s[i] >= 2*ste_avg)
            {
                /* Signal has started */
                frame_start = i;
                break;
            }
        }
    }

    window.clear();
    for(std::size_t i=s.size()-1; i>0; i--)
    {
        if(s.size()-i<=10)
        {
            window.push_back(s[i]);
            ste_avg = accumulate(window.begin(), window.end(), 0.0)/window.size();
        }
        else
        {
            if(s[i] >= 2*ste_avg)
            {
                /* Signal has ended */
                frame_end = i;
                break;
            }
        }
    }

    for(std::size_t i=frame_start; i<frame_end; i++)
    {
        out<<i-frame_start<<","<<s[i]<<","<<z[i]<<endl;
    }

    fin.close();
}

void recognizer(string yes_file, string no_file, string test_file)
{
    ifstream yin;
    ifstream nin;
    ifstream test;
    ofstream fout;
    string line;

    long double avg_zcr_yes=0, avg_zcr_no=0;
    long double avg_ste_yes=0, avg_ste_no=0;

    long double avg_ste_test=0, avg_zcr_test=0;


    yin.open("./Data/normalised/"+yes_file+".csv");
    nin.open("./Data/normalised/"+no_file+".csv");
    test.open("./Data/normalised/"+test_file+".csv");

    int y_limit=0, n_limit=0, test_limit=0;
    vector<long double> yes_ste_v;
    vector<long double> yes_zcr_v;
    vector<long double> no_ste_v;
    vector<long double> no_zcr_v;

    vector<long double> test_ste_v;
    vector<long double> test_zcr_v;

    while(getline(yin, line))
    {
        y_limit++;
        vector<string> temp;
        string word;
        stringstream stream(line);
        while( getline(stream, word, ',') )
            temp.push_back(word);

        yes_ste_v.push_back(stoi(temp[1]));
        yes_zcr_v.push_back(stoi(temp[2]));
    }
    while(getline(nin, line))
    {
        n_limit++;
        vector<string> temp;
        string word;
        stringstream stream(line);
        while( getline(stream, word, ',') )
            temp.push_back(word);

        no_ste_v.push_back(stoi(temp[1]));
        no_zcr_v.push_back(stoi(temp[2]));
    }
    while(getline(test, line))
    {
        test_limit++;
        vector<string> temp;
        string word;
        stringstream stream(line);
        while( getline(stream, word, ',') )
            temp.push_back(word);

        test_ste_v.push_back(stoi(temp[1]));
        test_zcr_v.push_back(stoi(temp[2]));
    }
    
    y_limit = ceil(y_limit*0.6);
    n_limit = ceil(n_limit*0.6);
    test_limit = ceil(test_limit*0.6);
    
    for(size_t i=yes_ste_v.size()-1; i>y_limit; i--)
    {
        avg_ste_yes += yes_ste_v[i];
    }
    for(size_t i=yes_zcr_v.size()-1; i>y_limit; i--)
    {
        avg_zcr_yes += yes_zcr_v[i];
    }
    for(size_t i=no_ste_v.size()-1; i>n_limit; i--)
    {
        avg_ste_no += no_ste_v[i];
    }
    for(size_t i=no_zcr_v.size()-1; i>n_limit; i--)
    {
        avg_zcr_no += no_zcr_v[i];
    }

    for(size_t i=test_ste_v.size()-1; i>test_limit; i--)
    {
        avg_ste_test += test_ste_v[i];
    }
    for(size_t i=test_zcr_v.size()-1; i>test_limit; i--)
    {
        avg_zcr_test += test_zcr_v[i];
    }

    avg_ste_yes = avg_ste_yes/ceil((y_limit*(1/0.6))*0.4);
    avg_zcr_yes = avg_zcr_yes/ceil((y_limit*(1/0.6))*0.4);
    avg_ste_no = avg_ste_no/ceil((n_limit*(1/0.6))*0.4);
    avg_zcr_no = avg_zcr_no/ceil((n_limit*(1/0.6))*0.4);

    avg_ste_test = avg_ste_test/ceil((test_limit*(1/0.6))*0.4);
    avg_zcr_test = avg_zcr_test/ceil((test_limit*(1/0.6))*0.4);


    cout<<"------------------------------ YES ------------------------------ "<<endl;
    cout<<"Yes STE = "<<avg_ste_yes<<endl;
    cout<<"Yes ZCR = "<<avg_zcr_yes<<endl;
    cout<<"------------------------------ NO ------------------------------ "<<endl;
    cout<<"No STE = "<<avg_ste_no<<endl;
    cout<<"No ZCR = "<<avg_zcr_no<<endl;
    cout<<"------------------------------ TEST ------------------------------ "<<endl;
    cout<<"Test STE = "<<avg_ste_test<<endl;
    cout<<"Test ZCR = "<<avg_zcr_test<<endl;

    cout<<"\n\n------------------------------ Final Answer ------------------------------ "<<endl;
    if(abs(avg_zcr_test-avg_zcr_yes) < abs(avg_zcr_test-avg_zcr_no))
    {
        if(abs(avg_ste_test-avg_ste_yes) < abs(avg_ste_test-avg_ste_no))
        {
            cout<<"YES"<<endl;
        }
        else
        {
            cout<<"Mostly YES"<<endl;
        }
    }
    else
    {
        if(abs(avg_ste_test-avg_ste_yes) > abs(avg_ste_test-avg_ste_no))
        {
            cout<<"NO"<<endl;
        }
        else
        {
            cout<<"Mostly NO"<<endl;
        }
    }
}

int main(int argc, char** argv)
{
    string yes_file = argv[1];
    string no_file = argv[2];
    string test_file = argv[3];
    
    train(yes_file);
    train(no_file);
    train(test_file);

    recognizer(yes_file, no_file, test_file);

    return 0;
}