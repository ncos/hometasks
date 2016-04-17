#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>

typedef long long int __int64;


const int cap = 32;  
std::vector<int> a; 
std::vector<int> table; 

char* str = "fuck it all!!!"; 
char* substr = "t ";

int x = rand()%20 + 2; 







int convert(int letter, std::vector<int> perm){   
    int out = 0;
    for(int n = 1; n<=8; n++){
        int sgn = abs((letter<<(cap-n))>>(cap-1));  
	    int mid = 1;  
	    if (sgn != 0){
            mid = mid<<(perm[n-1]-1);
            out = out | mid;
        };
    };
    return out;
};

__int64 convert_2(int letter){
    __int64 out = 0;
    for(int n = 1; n<=8; n++){
        int sgn = abs((letter<<(cap-n))>>(cap-1)); 
        out = out + sgn * pow(x, a[n-1]); 
    };
    return out;
};

__int64 gethash(std::string s1){
    int i = 0;
    __int64 hash = 0;
    
    while(s1[i] != '\0'){
        int letter = convert(s1[i], a);
        __int64 letter2 = convert_2(letter);
        hash = hash ^ (letter2 * table[letter]); 
        i++;
    };
    return hash;
};








int main(){

    table.resize(256);
    float p = 10; 
    for(int i = 0; i<=255; i++){
        for(int dv = 2; dv<=int(p/2); dv++){ 
            if(p/dv == int(p/dv)){
                p++;
                dv = 1;
            }; 
        };    
        table[i] = int(p); 
        p++;                 
    };
    
    for(int i = 0; i<=255; i++){
        int j = rand()%256;
        int old = table[i];
        table[i] = table[j];
        table[j] = old;
    };    

    a.resize(8);  
    for(int i = 0; i<=7; i++){
        a[i] = i+1;
    };
    
    for(int i = 0; i<=7; i++){
        int j = rand()%8;
        int old = a[i];
        a[i] = a[j];
        a[j] = old;
    };    

	

	
	int len =0; 
	for(int i =0; substr[i]!='\0'; i++){
        len++;
    };
    
    __int64 subhash = gethash(substr);
    

    for(int i = 0; i<=255; i++){
        std::cout<<table[i]<<"; ";
    };    
    std::cout<<"\nCurrent permutation is: \n";
    for(int i = 0; i<=7; i++){
        std::cout<<i<<" <-> "<<a[i]<<"\n"; 
    };
    
    std::cout<<"Current sting is: "<<str<<"\n";
    std::cout<<"Current substing is: "<<substr<<"\n";
    std::cout<<"Substring hash is: "<<subhash<<"\n";
    std::cout<<"Substring length is: "<<len<<"\n";
    
    std::cout<<">>>>>>>>: "<<gethash("abc")<<"\n";
    std::cout<<">>>>>>>>: "<<gethash("abd")<<"\n";
    std::cout<<">>>>>>>>: "<<(gethash("cab") ^ gethash("cd"))<<"\n";

    
    std::string s2 = "";
    
    for(int i = 0; i<=len-1; i++){
        s2 = s2+str[i];  
    };
    
    __int64 s2hash = gethash(s2);
    
    int i =0;
    while (str[i+len-1] != '\0'){
        if (subhash == s2hash){
            std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!: \n";
            
            s2 = "";
            for(int j = i; j<=len-1+i; j++){
            s2 = s2+str[j]; 
            };
            
            if (s2 == substr){
                std::cout<<"Substring position is "<<i+1<<"\n"; 
                break; };
        };
        
        
        
        s2 = "";
        for(int j = i; j<=len-1+i; j++){
        s2 = s2+str[j];  
        };
        
        std::cout<<"-----------> "<<s2<<"\n";
        std::cout<<"-->-->-->--> "<<gethash(s2)<<"\n";  
        std::cout<<"-----------> "<<s2hash<<"\n";
        
        
        s2 = str[i];
        s2hash = s2hash ^ gethash(s2);

   
        s2 = str[i+len];
        s2hash = s2hash ^ gethash(s2);
        i++;

    };






}
