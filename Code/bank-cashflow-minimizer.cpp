#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <unordered_map>
#include <limits.h>
using namespace std;

class bank{
    public:
    string name; // The name of the bank.
    int netAmount; // The net amount of money the bank has.
    set<string> paymentModes; // A set of payment modes supported by the bank.
}; 

//This function finds the bank with the minimum net amount among those that have non-zero balances.
//TC-O(numBanks)
int getMinIndex(bank listOfNetAmounts[],int numBanks){
    int min=INT_MAX, minIndex=-1;
    for(int i=0;i<numBanks;i++){
        if(listOfNetAmounts[i].netAmount == 0) continue;
        
        if(listOfNetAmounts[i].netAmount < min){
            minIndex = i;
            min = listOfNetAmounts[i].netAmount;
        }
    }
    return minIndex;
}

// This function finds the index of the bank with the maximum net amount, excluding banks with zero net amounts.
//TC-O(numBanks)
int getMaxIndex(bank listOfNetAmounts[],int numBanks){
    int max=INT_MIN, maxIndex=-1;
    for(int i=0;i<numBanks;i++){
        if(listOfNetAmounts[i].netAmount == 0) continue;
        
        if(listOfNetAmounts[i].netAmount > max){
            maxIndex = i;
            max = listOfNetAmounts[i].netAmount;
        }
    }
    return maxIndex;
}

// This function finds the index of the bank with the maximum net amount that has at least one common payment mode with the bank at the given minIndex. It also returns the common payment mode.
// TC-O(numBanks * maxPaymentModes)
pair<int,string> commonPaymentMaxIndex(bank listOfNetAmounts[],int numBanks,int minIndex,bank input[],int maxPaymentModes){
    int max=INT_MIN;
    int maxIndex=-1;
    string matchingType;
    
    for(int i=0;i<numBanks;i++){
        if(listOfNetAmounts[i].netAmount == 0) continue;
        
        if(listOfNetAmounts[i].netAmount < 0) continue;
        
        //TODO 
        //see complexity of intersection
        
        vector<string> v(maxPaymentModes);
        //set_intersection: This standard C++ function finds the common elements between two sorted ranges (types lists of two banks in this case) and stores the result in vector v.
        vector<string>::iterator ls=set_intersection(listOfNetAmounts[minIndex].paymentModes.begin(),listOfNetAmounts[minIndex].paymentModes.end(), listOfNetAmounts[i].paymentModes.begin(),listOfNetAmounts[i].paymentModes.end(), v.begin());
        //The result is stored in v, and ls points to the end of the common payment modes.
        if((ls-v.begin())!=0 && max < listOfNetAmounts[i].netAmount ){
            max=listOfNetAmounts[i].netAmount;
            maxIndex=i;
            matchingType = *(v.begin());
        } 
        // The number of common modes is calculated as (ls - v.begin()). If this value is not zero, it means there is at least one common payment mode.
    }
    
    //if there is NO such max which has a common type with any remaining banks then maxIndex has -1
    // also return the common payment type
    return make_pair(maxIndex,matchingType);
}


// This function prints the calculated transactions in a formatted manner.
void printAns(vector<vector<pair<int,string>>> ansGraph, int numBanks,bank input[]){
    
    cout<<"\nThe transactions for minimum cash flow are as follows : \n\n";
    for(int i=0;i<numBanks;i++){
        for(int j=0;j<numBanks;j++){
            
            if(i==j) continue;
            
            if(ansGraph[i][j].first != 0 && ansGraph[j][i].first != 0){
                
                if(ansGraph[i][j].first == ansGraph[j][i].first){
                    ansGraph[i][j].first=0;
                    ansGraph[j][i].first=0;
                }
                else if(ansGraph[i][j].first > ansGraph[j][i].first){
                    ansGraph[i][j].first -= ansGraph[j][i].first; 
                    ansGraph[j][i].first =0;
                    
                    cout<<input[i].name<<" pays Rs" << ansGraph[i][j].first<< "to "<<input[j].name<<" via "<<ansGraph[i][j].second<<endl;
                }
                else{
                    ansGraph[j][i].first -= ansGraph[i][j].first;
                    ansGraph[i][j].first = 0;
                    
                    cout<<input[j].name<<" pays Rs "<< ansGraph[j][i].first<<" to "<<input[i].name<<" via "<<ansGraph[j][i].second<<endl;
                    
                }
            }
            else if(ansGraph[i][j].first != 0){
                cout<<input[i].name<<" pays Rs "<<ansGraph[i][j].first<<" to "<<input[j].name<<" via "<<ansGraph[i][j].second<<endl;
                
            }
            else if(ansGraph[j][i].first != 0){
                cout<<input[j].name<<" pays Rs "<<ansGraph[j][i].first<<" to "<<input[i].name<<" via "<<ansGraph[j][i].second<<endl;
                
            }
            
            ansGraph[i][j].first = 0;
            ansGraph[j][i].first = 0;
        }
    }
    cout<<"\n";
}

// This is the main function that performs the cash flow minimization algorithm:
void minimizeCashFlow(int numBanks,bank input[],unordered_map<string,int>& indexOf,int numTransactions,vector<vector<int>>& graph,int maxPaymentModes){
    
    //Find net amount of each bank has
    bank listOfNetAmounts[numBanks];
    
    for(int b=0;b<numBanks;b++){
        listOfNetAmounts[b].name = input[b].name;
        listOfNetAmounts[b].paymentModes = input[b].paymentModes;
        
        int amount = 0;
        //incoming edges : Money that is credited to the bank.
        //column travers
        //Receive from i bank
        for(int i=0;i<numBanks;i++){
            amount += (graph[i][b]);
        }
        
        //outgoing edges : Money that the bank owes to others.
        //row traverse
        //Payment money to j bank
        for(int j=0;j<numBanks;j++){
            amount += ((-1) * graph[b][j]);
        }
        
        listOfNetAmounts[b].netAmount = amount;
    }

    
    //debitor -> creditor -> money -> mode
    vector<vector<pair<int,string>>> ansGraph(numBanks,vector<pair<int,string>>(numBanks,{0,""}));//adjacency matrix
    
    
    // No of banks with 0 net amount
    int numZeroNetAmounts=0;
    
    for(int i=0;i<numBanks;i++){
        if(listOfNetAmounts[i].netAmount == 0) numZeroNetAmounts++;
    }
    
    //TC-O(numBanks-1)*call getMaxIndex
    while(numZeroNetAmounts!=numBanks){
        
        int minIndex=getMinIndex(listOfNetAmounts, numBanks);
        pair<int,string> maxAns = commonPaymentMaxIndex(listOfNetAmounts, numBanks, minIndex,input,maxPaymentModes);
        
        int maxIndex = maxAns.first;
        
        //Actions to take when no common payment mode exists
        //Add the entire debt amount to World Bank
        if(maxIndex == -1){
            
            //Total amount
            (ansGraph[minIndex][0].first) += abs(listOfNetAmounts[minIndex].netAmount);
            //Payment Method
            (ansGraph[minIndex][0].second) = *(input[minIndex].paymentModes.begin());
            
            //Find the creditor without considering payment modes.
            int simpleMaxIndex = getMaxIndex(listOfNetAmounts, numBanks);
            //ransfer the debt from World bank to this creditor
            (ansGraph[0][simpleMaxIndex].first) += abs(listOfNetAmounts[minIndex].netAmount);
            (ansGraph[0][simpleMaxIndex].second) = *(input[simpleMaxIndex].paymentModes.begin());
            
            //Total net amount to the creditor bank 
            listOfNetAmounts[simpleMaxIndex].netAmount += listOfNetAmounts[minIndex].netAmount;
            //Total net amount to the previous debtor (i.e. before World bank which has minimum net amount)
            listOfNetAmounts[minIndex].netAmount = 0;
            
            if(listOfNetAmounts[minIndex].netAmount == 0) numZeroNetAmounts++;
            
            if(listOfNetAmounts[simpleMaxIndex].netAmount == 0) numZeroNetAmounts++;
            
        }
        //If a common payment mode exists (maxIndex != -1):
        else{
            int transactionAmount = min(abs(listOfNetAmounts[minIndex].netAmount), listOfNetAmounts[maxIndex].netAmount);
            
            //debitor -> creditor -> amount
            (ansGraph[minIndex][maxIndex].first) += (transactionAmount);
            //debitor -> creditor -> mode
            (ansGraph[minIndex][maxIndex].second) = maxAns.second;
            
            //add the transaction amount to the debitor to minimize the debit (-total(before) + +transaction(after))
            listOfNetAmounts[minIndex].netAmount += transactionAmount;
            listOfNetAmounts[maxIndex].netAmount -= transactionAmount;
            
            if(listOfNetAmounts[minIndex].netAmount == 0) numZeroNetAmounts++;
            
            if(listOfNetAmounts[maxIndex].netAmount == 0) numZeroNetAmounts++;
        }
        
    }
    
    printAns(ansGraph,numBanks,input);
}

//correct
int main() 
{ 
    cout<<"\n\t\t\t\t********************* Welcome to CASH FLOW MINIMIZER SYSTEM ***********************\n\n\n";
    cout<<"This system minimizes the number of transactions among multiple banks in the different corners of the world that use different modes of payment.There is one world bank (with all payment modes) to act as an intermediary between banks that have no common mode of payment. \n\n";
    cout<<"Enter the number of banks participating in the transactions.\n";
    int numBanks;
    cin>>numBanks;
    
    bank input[numBanks];
    unordered_map<string,int> indexOf;//stores index of a bank
    
    cout<<"Enter the details of the banks and transactions as stated:\n";
    cout<<"Bank name ,number of payment modes it has and the payment modes.\n";
    cout<<"Bank name and payment modes should not contain spaces\n";
    
    int maxPaymentModes;
    for(int i=0; i<numBanks;i++){
        if(i==0){
            cout<<"World Bank : ";
        }
        else{
            cout<<"Bank "<<i<<" : ";
        }
        cin>>input[i].name;
        indexOf[input[i].name] = i;
        int numTypes;
        cin>>numTypes;
        
        if(i==0) maxPaymentModes = numTypes;
        
        string type;
        while(numTypes--){
            cin>>type;
            
            input[i].paymentModes.insert(type);
        }   
        
    }
    
    cout<<"Enter number of transactions.\n";
    int numTransactions;
    cin>>numTransactions;
    
    //Adjacency Matrix
    vector<vector<int>> graph(numBanks,vector<int>(numBanks,0));
    
    cout<<"Enter the details of each transaction as stated:";
    cout<<"Debtor Bank , creditor Bank and amount\n";
    cout<<"The transactions can be in any order\n";
    for(int i=0;i<numTransactions;i++){
        cout<<(i)<<" th transaction : ";
        string s1,s2;
        int amount;
        cin >> s1>>s2>>amount;
        

        //Debitor -> Creditor = amount
        graph[indexOf[s1]][indexOf[s2]] = amount;
    }
     
    //Solve
    minimizeCashFlow(numBanks,input,indexOf,numTransactions,graph,maxPaymentModes);
    return 0; 
} 

//Total Time Complexity = O(numBanks^2 * maxPaymentModes)
//Total Space Comlexity = O(numBanks^2 * maxPaymentModes)


/*
                                ********************* Welcome to CASH FLOW MINIMIZER SYSTEM ***********************


This system minimizes the number of transactions among multiple banks in the different corners of the world that use different modes of payment.There is one world bank (with all payment modes) to act as an intermediary between banks that have no common mode of payment.

Enter the number of banks participating in the transactions.
6
Enter the details of the banks and transactions as stated:
Bank name ,number of payment modes it has and the payment modes.
Bank name and payment modes should not contain spaces
World Bank : WB 3 GPay AliPay Paytm
Bank 1 : B1 2 GPay AliPay
Bank 2 : B2 1 AliPay
Bank 3 : B3 2 GPay AliPay
Bank 4 : B4 1 Paytm
Bank 5 : B5 2 AliPay Paytm
Enter number of transactions.
9
Enter the details of each transaction as stated:Debtor Bank , creditor Bank and amount
The transactions can be in any order
0 th transaction : B4 WB 100
1 th transaction : B4 B1 300
2 th transaction : B4 B2 100
3 th transaction : B4 B3 100
4 th transaction : B5 WB 300
5 th transaction : B5 B2 100
6 th transaction : WB B1 400
7 th transaction : B2 B3 500
8 th transaction : B1 B2 200

The transactions for minimum cash flow are as follows :

WB pays Rs 600 to B3 via AliPay
B4 pays Rs 600 to WB via Paytm
B2 pays Rs 100 to B1 via AliPay
B5 pays Rs 400 to B1 via AliPay

*/