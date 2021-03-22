//
//  main.cpp
//  QTS DYNAMIC SLIDING WINDOW
//
//  Created by 唐健恆 on 2021/3/14.
//  Copyright © 2021 Alvin. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <cfloat>
#include "date.h"
#include "portfolio.h"


using namespace std;
using namespace __fs::filesystem;

#define EXPNUMBER 50
#define ITERNUMBER 10000
#define PORTFOLIONUMBER 10
#define FUNDS 10000000
#define DELTA 0.0004
#define QTSTYPE 2 //QTS 0, GQTS 1, GNQTS 2
#define TRENDLINETYPE 0 //linear 0, quadratic 1

string file_dir = "0322_1130";
string MODE = "test";

bool readData(string filename, vector<vector<string>> &data_vector, int &size, int &day_number) {
    cout << filename << endl;
    ifstream inFile(filename, ios::in);
    string line;
    vector< vector<string> > temp;

    if (!inFile) {
        cout << "Open file failed!" << endl;
        exit(1);
    }
    while (getline(inFile, line)) {
        istringstream delime(line);
        string s;
        vector<string> line_data;
        while (getline(delime, s, ',')) {
            if (s != "\r") {
                s.erase(remove(s.begin(), s.end(), '\r'), s.end());
                line_data.push_back(s);
            }
        }
        temp.push_back(line_data);
    }
    inFile.close();
    
    size = temp[0].size() - 1;
    day_number = temp.size() - 1;
    data_vector = temp;
//    string **d = new string*[temp.size()];
//    for(int j = 0; j < temp.size(); j++){
//        d[j] = new string[temp[0].size()];
//        for(int k = 0; k < temp[0].size(); k++){
//            d[j][k] = temp[j][k];
//        }
//    }

    return true;
}

bool readSpeData(string filename, string title, vector<string> &myTrainData_vector, int &myTrainData_size) {
    cout << filename << endl;
    ifstream inFile(filename, ios::in);
    string line;
    vector< vector<string> > data;

    if (!inFile) {
        cout << "Open file failed!" << endl;
        return false;
    }
    
    bool sw = false;
    vector<string> line_data;
    while(getline(inFile, line)){
        istringstream delime(line);
        string s;
        
        while(getline(delime, s, ',')){
            if(sw){
                if(s != "\r"){
                    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
                    line_data.push_back(s);
                }
            }
            if(s == title){
                sw = true;
            }
        }
        sw = false;
    }
    inFile.close();
    
    myTrainData_size = line_data.size();
    myTrainData_vector = line_data;
//    myTrainData = new string[myTrainData_size];
//    for(int j = 0; j < myTrainData_size; j++){
//        myTrainData[j] = line_data[j];
//    }
    return true;
}

string** vectorToArray(vector<vector<string>> data_vector){
    string **d = new string*[data_vector.size()];
    for(int j = 0; j < data_vector.size(); j++){
        d[j] = new string[data_vector[0].size()];
        for(int k = 0; k < data_vector[0].size(); k++){
            d[j][k] = data_vector[j][k];
        }
    }
    return d;
}

string* vectorToArray(vector<string> myTrainData_vector){
    string *d = new string[myTrainData_vector.size()];
    for(int j = 0; j < myTrainData_vector.size(); j++){
        d[j] = myTrainData_vector[j];
    }
    return d;
}

void createStock(Stock* stock_list, int size, int range_day_number, string **data, int start_index, int end_index) {
    for (int j = 0; j < size; j++) {
        stock_list[j].idx = j;
        stock_list[j].init(range_day_number + 1);
        stock_list[j].company_name = data[0][j+1];
        for (int k = start_index - 1; k <= end_index; k++) {
            stock_list[j].price_list[k - start_index + 1] = atof(data[k][j+1].c_str());
        }
    }
}

void preSet(string mode, Date& current_date, Date& finish_date, int SLIDETYPE, string& TYPE) {
    string STARTYEAR;
    string STARTMONTH;
    string ENDYEAR;
    string ENDMONTH;
    int slide_number;
    int train_range;
    switch (SLIDETYPE) {
    case 0:
        STARTYEAR = "2009";
        STARTMONTH = "12";
        ENDYEAR = "2019";
        ENDMONTH = "11";
        TYPE = "M2M";
        train_range = 1;
        slide_number = 1;
        break;
    case 1:
        STARTYEAR = "2009";
        STARTMONTH = "10";
        ENDYEAR = "2019";
        ENDMONTH = "9";
        TYPE = "Q2M";
        train_range = 3;
        slide_number = 1;
        break;
    case 2:
        STARTYEAR = "2009";
        STARTMONTH = "10";
        ENDYEAR = "2019";
        ENDMONTH = "7";
        TYPE = "Q2Q";
        train_range = 3;
        slide_number = 3;
        break;
    case 3:
        STARTYEAR = "2009";
        STARTMONTH = "7";
        ENDYEAR = "2019";
        ENDMONTH = "6";
        TYPE = "H2M";
        train_range = 6;
        slide_number = 1;
        break;
    case 4:
        STARTYEAR = "2009";
        STARTMONTH = "7";
        ENDYEAR = "2019";
        ENDMONTH = "4";
        TYPE = "H2Q";
        train_range = 6;
        slide_number = 3;
        break;
    case 5:
        STARTYEAR = "2009";
        STARTMONTH = "7";
        ENDYEAR = "2019";
        ENDMONTH = "1";
        TYPE = "H2H";
        train_range = 6;
        slide_number = 6;
        break;
    case 6:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "12";
        TYPE = "Y2M";
        train_range = 12;
        slide_number = 1;
        break;
    case 7:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "10";
        TYPE = "Y2Q";
        train_range = 12;
        slide_number = 3;
        break;
    case 8:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "7";
        TYPE = "Y2H";
        train_range = 12;
        slide_number = 6;
        break;
    case 9:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "1";
        TYPE = "Y2Y";
        train_range = 12;
        slide_number = 12;
        break;
    case 10:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "12";
        TYPE = "M#";
        if(mode == "train"){
            train_range = 1;
        }else{
            train_range = 12;
        }
        slide_number = 1;
        break;
    case 11:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "10";
        TYPE = "Q#";
        if(mode == "train"){
            train_range = 3;
        }else{
            train_range = 12;
        }
        slide_number = 3;
        break;
    case 12:
        STARTYEAR = "2009";
        STARTMONTH = "1";
        ENDYEAR = "2018";
        ENDMONTH = "7";
        TYPE = "H#";
        if(mode == "train"){
            train_range = 6;
        }else{
            train_range = 12;
        }
        slide_number = 6;
        break;
    }

    current_date.date.tm_year = atoi(STARTYEAR.c_str()) - 1900;
    current_date.date.tm_mon = atoi(STARTMONTH.c_str()) - 1;
    current_date.date.tm_mday = 1;
    current_date.slide_number = slide_number;
    current_date.train_range = train_range;
    
    finish_date.date.tm_year = atoi(ENDYEAR.c_str()) - 1900;
    finish_date.date.tm_mon = atoi(ENDMONTH.c_str()) - 1;
    finish_date.date.tm_mday = 1;
    finish_date.slide_number = slide_number;
    finish_date.train_range = train_range;
    
    if(mode == "test"){
        current_date.slide(train_range);
        finish_date.slide(train_range);
    }
}

void createDir(string file_dir, string type){
    create_directory(file_dir);
    create_directory(file_dir + "/" + type);
    create_directory(file_dir + "/" + type + "/train");
    create_directory(file_dir + "/" + type + "/test");
}

void copyData(string *data_copy, string **data, int day_number){
    for(int j = 0; j < day_number; j++){
        data_copy[j] = data[j+1][0];
        data_copy[j].resize(6);
    }
}

void setWindow(string mode, string &start_date, string &end_date, int &start_index, int &end_index, Date current_date, Date finish_data, string* data_copy, string **data, int day_number, int &range_day_number){
    bool sw = false;//判斷是否找到開始日期
    int flag = 0;//判斷是否找到結束月份
    for(int j = 0; j < day_number; j++){
        string temp1 = current_date.getYear() + current_date.getMon();
        string temp2;
        if(mode == "train"){
            temp2 = current_date.getRangeEnd(current_date.train_range - 1).getYear() + current_date.getRangeEnd(current_date.train_range - 1).getMon();
        }else{
            temp2 = current_date.getRangeEnd(current_date.slide_number - 1).getYear() + current_date.getRangeEnd(current_date.slide_number - 1).getMon();
        }
        
        if(!sw && temp1 == data_copy[j]){
            start_date = data[j+1][0];
            start_index = j + 1;
            sw = true;
        }
        
        if(sw){
            if(flag == 1 && temp2 != data_copy[j]){
                end_date = data[j][0];
                end_index = j;
                flag = 0;
                break;
            }
            if(flag == 0 && temp2 == data_copy[j]){
                flag = 1;
            }
        }
    }
    if(flag == 1){
        end_date = data[day_number][0];
    }
    range_day_number = end_index - start_index + 1;
}

void initial(double *b, int size) {
    for (int j = 0; j < size; j++) {
        b[j] = 0.5;
    }
}

void initPortfolio(Portfolio *p, int size, int day_number){
    for(int j = 0; j < PORTFOLIONUMBER; j++){
        p[j].init(size, day_number, FUNDS);
    }
}

void genPortfolio(Portfolio* portfolio_list, Stock* stock_list, int portfolio_number, double *beta_, string mode, int n, int i) {
    for (int j = 0; j < portfolio_number; j++) {
        portfolio_list[j].exp = n + 1;
        portfolio_list[j].gen = i + 1;
        portfolio_list[j].stock_number = 0;
        for (int k = 0; k < portfolio_list[j].size; k++) {
            double r = (double)rand() / (double)RAND_MAX;
            if (r > beta_[k]) {
                portfolio_list[j].data[k] = 0;
            }
            else {
                portfolio_list[j].data[k] = 1;
            }
        }
        
        for(int k = 0; k < portfolio_list[j].size; k++){
            if(portfolio_list[j].data[k] == 1){
                portfolio_list[j].constituent_stocks[portfolio_list[j].stock_number] = stock_list[k];
                portfolio_list[j].stock_number++;
            }
        }
    }
}

void gen_testPortfolio(Portfolio* portfolio_list, Stock* stock_list, int portfolio_number, string mode, string **data, string *myTrainData, int myTrainData_size) {
    for (int j = 0; j < portfolio_number; j++) {
        for(int k = 0; k < portfolio_list[j].size; k++){
            for(int h = 0; h < myTrainData_size; h++){
                if(data[0][k+1] == myTrainData[h]){
                    portfolio_list[j].data[k] = 1;
                    portfolio_list[j].constituent_stocks[portfolio_list[j].stock_number] = stock_list[k];
                    portfolio_list[j].stock_number++;
                    break;
                }
            }
        }
    }
}

void capitalLevel(Portfolio* portfolio_list, int portfolio_number, double funds) {
    for (int j = 0; j < portfolio_number; j++) {
        for (int k = 0; k < portfolio_list[j].stock_number; k++) {
            portfolio_list[j].investment_number[k] = portfolio_list[j].getDMoney() / portfolio_list[j].constituent_stocks[k].price_list[0];
            portfolio_list[j].remain_fund[k] = portfolio_list[j].getDMoney() - (portfolio_list[j].investment_number[k] * portfolio_list[j].constituent_stocks[k].price_list[0]);
        }
//        portfolio_list[j].total_money[0] = funds;
        for (int k = 0; k < portfolio_list[j].day_number; k++) {
            portfolio_list[j].total_money[k] = portfolio_list[j].getRemainMoney();
            for (int h = 0; h < portfolio_list[j].stock_number; h++) {
                portfolio_list[j].total_money[k] += portfolio_list[j].investment_number[h] * portfolio_list[j].constituent_stocks[h].price_list[k + 1];
            }
        }
    }
}

void countTrend(Portfolio* portfolio_list, int porfolio_number, double funds) {
    
    for (int j = 0; j < porfolio_number; j++) {
        double sum = 0;
        if (TRENDLINETYPE == 0) {
            //portfolio_list[j].countQuadraticYLine();
            double x = 0;
            double y = 0;
            for (int k = 0; k < portfolio_list[j].day_number; k++) {
                x += (k + 1) * (portfolio_list[j].total_money[k] - funds);
                y += (k + 1) * (k + 1);
            }
            if (portfolio_list[j].stock_number != 0) {
                portfolio_list[j].m = x / y;
            }
            for (int k = 0; k < portfolio_list[j].day_number; k++) {
                double Y;
                Y = portfolio_list[j].getNormalY(k + 1);
                sum += (portfolio_list[j].total_money[k] - Y) * (portfolio_list[j].total_money[k] - Y);
            }
        }
        else if (TRENDLINETYPE == 1) {
            portfolio_list[j].countQuadraticYLine();
            for (int k = 0; k < portfolio_list[j].day_number; k++) {
                double Y;
                Y = portfolio_list[j].getQuadraticY(k + 1);
                sum += (portfolio_list[j].total_money[k] - Y) * (portfolio_list[j].total_money[k] - Y);
            }
            portfolio_list[j].m = (portfolio_list[j].getQuadraticY(portfolio_list[j].day_number) - portfolio_list[j].getQuadraticY(1)) / (portfolio_list[j].day_number - 1);
        }
        
        portfolio_list[j].daily_risk = sqrt(sum / (portfolio_list[j].day_number));

        if (portfolio_list[j].m < 0) {
            portfolio_list[j].trend = portfolio_list[j].m * portfolio_list[j].daily_risk;
        }
        else {
            portfolio_list[j].trend = portfolio_list[j].m / portfolio_list[j].daily_risk;
        }
    }
}

void recordGAnswer(Portfolio* portfolio_list, Portfolio& gBest, Portfolio& gWorst, Portfolio& pBest, Portfolio& pWorst) {
    pBest.copyP(portfolio_list[0]);
    pWorst.copyP(portfolio_list[PORTFOLIONUMBER - 1]);
    for (int j = 0; j < PORTFOLIONUMBER; j++) {
        if (pBest.trend < portfolio_list[j].trend) {
            pBest.copyP(portfolio_list[j]);
        }
        if (pWorst.trend > portfolio_list[j].trend) {
            pWorst.copyP(portfolio_list[j]);
        }
    }
    
    if (gBest.trend < pBest.trend) {
            gBest.copyP(pBest);
    }
    
    if (gWorst.trend > pWorst.trend) {
        gWorst.copyP(pWorst);
    }
}

void adjBeta(Portfolio& best, Portfolio& worst, double *beta_) {
    for (int j = 0; j < best.size; j++) {
        if (QTSTYPE == 2) {
            if (best.data[j] > worst.data[j]) {
                if (beta_[j] < 0.5) {
                    beta_[j] = 1 - beta_[j];
                }
                beta_[j] += DELTA;
            }
            else if (best.data[j] < worst.data[j]) {
                if (beta_[j] > 0.5) {
                    beta_[j] = 1 - beta_[j];
                }
                beta_[j] -= DELTA;
            }
        }
        else if (QTSTYPE == 1) {
            if (best.data[j] > worst.data[j]) {
                beta_[j] += DELTA;
            }
            else if (best.data[j] < worst.data[j]) {
                beta_[j] -= DELTA;
            }
        }
        else {
            if (best.data[j] > worst.data[j]) {
                beta_[j] += DELTA;
            }
            else if (best.data[j] < worst.data[j]) {
                beta_[j] -= DELTA;
            }
        }
    }
}

void recordExpAnswer(Portfolio& expBest, Portfolio& gBest) {
    if (expBest.trend < gBest.trend) {
        expBest.copyP(gBest);
        expBest.answer_counter = 1;
    }
    else if (expBest.trend == gBest.trend) {
        expBest.answer_counter++;
    }
}

string getOutputFilePath(Date current_date, string mode, string file_dir, string type){
    return file_dir + "/" + type + "/" + mode + "/" + mode + "_" + current_date.getYear() + "_" + current_date.getMon() + ".csv";
}

void outputFile(Date current_date, Portfolio& portfolio, string mode, string file_name, string **data, int start_index) {
    ofstream outfile;
    outfile.open(file_name, ios::out);
    outfile << setprecision(15);

    outfile << "Iteration," << ITERNUMBER << endl;
    outfile << "Element number," << PORTFOLIONUMBER << endl;
    outfile << "Delta," << DELTA << endl;
    outfile << "Exp times," << EXPNUMBER << endl << endl;
    
    outfile << "Init funds," << portfolio.funds << endl;
    outfile << "Final funds," << portfolio.total_money[portfolio.day_number - 1] << endl;
    outfile << "Real award," << portfolio.total_money[portfolio.day_number - 1] - portfolio.funds << endl << endl;
    
    outfile << "m," << portfolio.m << endl;
    outfile << "Daily_risk," << portfolio.daily_risk << endl;
    outfile << "Trend," << portfolio.trend << endl << endl;
    
    if (TRENDLINETYPE == 0) {
        portfolio.countQuadraticYLine();
        double sum = 0;
        for (int k = 0; k < portfolio.day_number; k++) {
            double Y;
            Y = portfolio.getQuadraticY(k + 1);
            sum += (portfolio.total_money[k] - Y) * (portfolio.total_money[k] - Y);
        }
        double c = (portfolio.getQuadraticY(portfolio.day_number) - portfolio.getQuadraticY(1)) / (portfolio.day_number - 1);
        double d = sqrt(sum / (portfolio.day_number));
        
        outfile << "Quadratic trend line," << portfolio.a << "x^2 + " << portfolio.b << "x + " << FUNDS << endl << endl;
        outfile << "Quadratic m," << c << endl;
        outfile << "Quadratic daily risk," << d << endl;
        if(c < 0){
            outfile << "Quadratic trend," << c * d << endl << endl;
        }else{
            outfile << "Quadratic trend," << c / d << endl << endl;
        }
    }
    else {
        outfile << "Quadratic trend line," << portfolio.a << "x^2 + " << portfolio.b << "x + " << FUNDS << endl << endl;
        double x = 0;
        double y = 1;
        double sum = 0;
        for (int k = 0; k < portfolio.day_number - 1; k++) {
            x += (k + 2) * (portfolio.total_money[k + 1] - portfolio.funds);
            y += (k + 2) * (k + 2);
        }

        double c = x / y;
        for (int k = 0; k < portfolio.day_number; k++) {
            double Y;
            Y = c * (k + 1) + portfolio.funds;
            sum += (portfolio.total_money[k] - Y) * (portfolio.total_money[k] - Y);
        }
        double d = sqrt(sum / (portfolio.day_number));

        outfile << "Linear m," << c << endl;
        outfile << "Linear daily risk," << d << endl;
        if(c < 0){
            outfile << "Linear trend," << c * d << endl << endl;
        }else{
            outfile << "Linear trend," << c / d << endl << endl;
        }
    }

    outfile << "Best generation," << portfolio.gen << endl;
    outfile << "Best experiment," << portfolio.exp << endl;
    outfile << "Best answer times," << portfolio.answer_counter << endl << endl;

    outfile << "Stock number," << portfolio.stock_number << endl;
    outfile << "Stock#,";
    for (int j = 0; j < portfolio.stock_number; j++) {
        outfile << portfolio.constituent_stocks[j].company_name << ",";
    }
    outfile << endl;

    outfile << "Number,";
    for (int j = 0; j < portfolio.stock_number; j++) {
        outfile << portfolio.investment_number[j] << ",";
    }
    outfile << endl;

    outfile << "Distribue funds,";
    for (int j = 0; j < portfolio.stock_number; j++) {
        outfile << portfolio.getDMoney() << ",";
    }
    outfile << endl;

    outfile << "Remain funds,";
    for (int j = 0; j < portfolio.stock_number; j++) {
        outfile << portfolio.remain_fund[j] << ",";
    }
    outfile << endl;

    for (int j = 0; j < portfolio.day_number; j++) {
        outfile << data[start_index + j][0] << ",";
        for (int k = 0; k < portfolio.stock_number; k++) {
            outfile << (portfolio.constituent_stocks[k].price_list[j + 1] * portfolio.investment_number[k]) + portfolio.remain_fund[k] << ",";
        }
        outfile << portfolio.total_money[j] << endl;
    }
    outfile << endl;
    outfile.close();

}

void recordCPUTime(double START, double END, string TYPE){
    double total_time = (END - START) / CLOCKS_PER_SEC;
    if(MODE == "train"){
        ofstream outfile_time;
        string file_name = file_dir + "/" + TYPE + "/" + "time.txt";
        outfile_time.open(file_name, ios::out);
        outfile_time << "total time: " << total_time << " sec" << endl;
    }
}

int main(int argc, const char * argv[]) {
    int size;
    int day_number;
    double START, END;
    Date current_date;
    Date finish_date;
    string TYPE;
    string** data;
    vector<vector<string>> data_vector;
    readData("20081231-2019.csv", data_vector, size, day_number);
    data = vectorToArray(data_vector);
    double *beta_ = new double[size];
    string *data_copy = new string[day_number];
    copyData(data_copy, data, day_number);
    
    for(int s = 0; s < 13; s++){
        srand(114);
        int current_funds = FUNDS;
        preSet(MODE, current_date, finish_date, s, TYPE);
        START = clock();
        createDir(file_dir, TYPE);
        cout << TYPE << endl;
        do{
            string start_date;
            string end_date;
            int start_index;
            int end_index;
            int range_day_number;
            setWindow(MODE, start_date, end_date, start_index, end_index, current_date, finish_date, data_copy, data, day_number, range_day_number);
            Stock* stock_list = new Stock[size];
            createStock(stock_list, size, range_day_number, data, start_index, end_index);
            
            if(MODE == "train"){
                
                Portfolio* portfolio_list = new Portfolio[PORTFOLIONUMBER];
                Portfolio gBest(size, range_day_number, FUNDS);
                Portfolio gWorst(size, range_day_number, FUNDS);
                Portfolio expBest(size, range_day_number, FUNDS);
                cout << TYPE << " " << start_date << " - " << end_date << endl;
                for(int n = 0; n < EXPNUMBER; n++){
                    cout << "___" << n << "___" << endl;
                    gBest.trend = 0;
                    gWorst.trend = DBL_MAX;
                    initial(beta_, size);
                    for(int i = 0; i < ITERNUMBER; i++){
                        Portfolio pBest(size, range_day_number, FUNDS);
                        Portfolio pWorst(size, range_day_number, FUNDS);
                        initPortfolio(portfolio_list, size, range_day_number);
                        genPortfolio(portfolio_list, stock_list, PORTFOLIONUMBER, beta_, MODE, n, i);
                        capitalLevel(portfolio_list, PORTFOLIONUMBER, FUNDS);
                        countTrend(portfolio_list, PORTFOLIONUMBER, FUNDS);
                        recordGAnswer(portfolio_list, gBest, gWorst, pBest, pWorst);
                        adjBeta(gBest, pWorst, beta_);
                    }
                    recordExpAnswer(expBest, gBest);
                }
                if(expBest.trend != 0){
                    outputFile(current_date, expBest, "train", getOutputFilePath(current_date, MODE, file_dir, TYPE), data, start_index);
                }
                delete[] portfolio_list;
                cout << endl;
                
            }else{
                string* myTrainData;
                vector<string> myTrainData_vector;
                int myTrainData_size;
                if(readSpeData(getOutputFilePath(current_date.getRangeEnd(-1 * current_date.train_range) , "train", file_dir, TYPE), "Stock#", myTrainData_vector, myTrainData_size)){
                    if(s==1){
                        cout << stock_list[0].price_list[0] << endl;
                    }
                    myTrainData = vectorToArray(myTrainData_vector);
                    Portfolio* test_portfolio = new Portfolio[1];
                    test_portfolio[0].init(size, range_day_number, current_funds);
                    gen_testPortfolio(test_portfolio, stock_list, 1, MODE, data, myTrainData, myTrainData_size);
                    test_portfolio[0].funds = current_funds;
                    capitalLevel(test_portfolio, 1, current_funds);
                    countTrend(test_portfolio, 1, current_funds);
                    current_funds = test_portfolio[0].total_money[test_portfolio[0].day_number - 1];
                    outputFile(current_date, test_portfolio[0], "test", getOutputFilePath(current_date, MODE, file_dir, TYPE),data, start_index);
                    delete[] test_portfolio;
                }
            }
            
            delete[] stock_list;
            current_date.slide();
        }while(finish_date >= current_date);
        END = clock();
        if(MODE == "train"){
            recordCPUTime(START, END, TYPE);
        }
        
    }
    return 0;
}
