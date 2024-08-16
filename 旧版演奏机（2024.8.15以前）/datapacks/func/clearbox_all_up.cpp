#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
using namespace std;
const string name="clearbox_all_up.mcfunction";//默认文件名 
//const int key[25]={1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1};//1为黑键，0为白键
int main(){
	fstream fout;
	fout.open(name.c_str(),ios::out);
	for(int i=0;i!=25;++i){
		for(int j=0;j!=54;++j){
			fout<<"item replace block ^-"<<(i+1)<<" ^2 ^"<<((j<27)?3:4)<<" container."<<((j<27)?j:(j-27))
			<<" with minecraft:air"<<endl;
		}
	}
	fout.close();
	return 0;
}
