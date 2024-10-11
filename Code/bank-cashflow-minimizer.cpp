#include <bits/stdc++.h>
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
// TC-O(numBanks * maxNumTypes)
pair<int,string> commonPaymentMaxIndex(bank listOfNetAmounts[],int numBanks,int minIndex,bank input[],int maxNumTypes){
    int max=INT_MIN;
    int maxIndex=-1;
    string matchingType;
    
    for(int i=0;i<numBanks;i++){
        if(listOfNetAmounts[i].netAmount == 0) continue;
        
        if(listOfNetAmounts[i].netAmount < 0) continue;
        
        //TODO 
        //see complexity of intersection
        
        vector<string> v(maxNumTypes);
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
void minimizeCashFlow(int numBanks,bank input[],unordered_map<string,int>& indexOf,int numTransactions,vector<vector<int>>& graph,int maxNumTypes){
    
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
        pair<int,string> maxAns = commonPaymentMaxIndex(listOfNetAmounts, numBanks, minIndex,input,maxNumTypes);
        
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
    int numBanks;cin>>numBanks;
    
    bank input[numBanks];
    unordered_map<string,int> indexOf;//stores index of a bank
    
    cout<<"Enter the details of the banks and transactions as stated:\n";
    cout<<"Bank name ,number of payment modes it has and the payment modes.\n";
    cout<<"Bank name and payment modes should not contain spaces\n";
    
    int maxNumTypes;
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
        
        if(i==0) maxNumTypes = numTypes;
        
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
    minimizeCashFlow(numBanks,input,indexOf,numTransactions,graph,maxNumTypes);
    return 0; 
} 


/*
5
A 2 t1 t2
B 1 t1
C 1 t1
D 1 t2
E 1 t2
4
B A 300
C A 700
D B 500
E B 500

--------
5
World_Bank 2 Google_Pay PayTM
Bank_B 1 Google_Pay
Bank_C 1 Google_Pay
Bank_D 1 PayTM
Bank_E 1 PayTM
4
Bank_B World_Bank 300
Bank_C World_Bank 700
Bank_D Bank_B 500
Bank_E Bank_B 500

--------------------

6
B 3 1 2 3
C 2 1 2
D 1 2
E 2 1 3
F 1 3
G 2 2 3
9
G B 30
G D 10
F B 10
F C 30
F D 10
F E 10
B C 40
C D 20
D E 50
*/