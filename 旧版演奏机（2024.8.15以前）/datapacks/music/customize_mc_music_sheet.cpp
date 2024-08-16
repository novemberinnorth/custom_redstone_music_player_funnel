#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
using namespace std;
const string default_name="setnote.mcfunction";//默认文件名 
const int key[25]={1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1};//1为黑键，0为白键 
int box[25][54]={{0}};//保存所有箱子的内容 
int isPlayed[25]={0};//某个音符是否演奏过 
int currentp[25]={0};//当前箱子存放序列的末端 
int playing[25]={0};//当前要同时演奏的音符
bool isFileInput=0;//是否为从文件输入 
string line;//输入的一行 
string name;//文件名
string input;//读取的文件名 
string choice;//用于保存用户选择 
fstream fout;
FILE* f;
void initialize(){//初始化所有变量数组 
	for(int i=0;i!=25;++i){
		isPlayed[i]=0,currentp[i]=0,playing[i]=0;
		for(int j=0;j!=54;++j){
			box[i][j]=0;
		}
	}
}
void clear_playing(){//清空playing，准备下一组输入 
	for(int i=0;i!=25;++i){
		playing[i]=0;
	}
}
int setbox(){//根据playing向箱子填充物品 
	for(int i=0;i!=25;++i){
		if(playing[i]){//要演奏，先步进到下一个空格，再放剪刀，再步进到下一个空格 
			if(box[i][currentp[i]]!=0){
				++currentp[i];
				if(currentp[i]==54){
					cout<<"Error:box is full, press enter to show the results."<<endl;
					system("pause");
					return -1;
				} 
			}
			box[i][currentp[i]]=-1;
			++currentp[i];
			if(currentp[i]==54){
				cout<<"Error:box is full, press enter to show the results."<<endl;
				system("pause");
				return -1;
			} 
		}else{//否则，在当前位置加一个泥土，若已满64个，则先步进到下一格 
			if(box[i][currentp[i]]==64){
				++currentp[i];
				if(currentp[i]==54){
					cout<<"Error:box is full, press enter to show the results."<<endl;
					system("pause");
					return -1;
				} 
			}
			++box[i][currentp[i]];
		}
	}
	return 0;
}
int main(){
	initialize();
	cout<<"Input file's name without suffix (press enter to use the default name) ."<<endl;
	getline(cin,name);
	if(name.size()==0){
		name=default_name;
	}else{
		name+=".mcfunction";
	}
	cout<<"Input file stream's mode, with 'c' for clearing up and 'a' for appending (press enter to use 'c' as default) ."<<endl;
	string smode;
	int mode;
	getline(cin,smode);
	if((smode.size()==0)||(smode=="c")||(smode=="C")){
		mode=1;
	}else{
		mode=0;
	}
z:	cout<<"Rule: Each line can be filled with multiple notes. Enter 0~24 as #F(8-)~#F(8+), -1 as pause and -2 to end.\n"
	<<"Do you want to input from a file? Press any key to input file's name or just enter to continue inputting from keyboard."<<endl;
	getline(cin,choice);
	if(choice.size()>0){
		isFileInput=1;
		cout<<"Please make sure your file follows the rule above. Enter the file's name (including suffix) ."<<endl;
		cin>>input;
		f=freopen(input.c_str(),"r",stdin);
		if(f==NULL){
			cout<<"Error:fail to open the file."<<endl;
			goto z;
		}
	}
	bool b=1,normend=0;
	int temp=0;
	while(b){
		getline(cin,line);
		for(auto i=0;i!=line.size();++i){//提取数字并执行相应操作，仅支持-2、-1及0~24 
			if((line[i]-'0'>=0)&&(line[i]-'0'<=9)){
				if(temp==0){
					temp=line[i]-'0';
				}else{
					temp=temp*10+line[i]-'0';
				}
			}else if(line[i]==' '){
				if(temp>24){
					cout<<"Error:number too large, press enter to show the results."<<endl;
					system("pause");
					b=0;
					break;
				}
				playing[temp]=1,isPlayed[temp]=1;
				temp=0;
			}else if(line[i]=='-'){//-1为休止一个时值，-2为正常结束输入 
				if((i+1)!=line.size()){
					if(line[i+1]=='1'){
						goto x;
					}else if(line[i+1]=='2'){
						b=0;
						normend=1;
						break;
					}else{
						cout<<"Error:number too small, press enter to show the results."<<endl;
						system("pause");
						b=0;
						break;
					}
				}
			}else{
				cout<<"Error:invalid char, press enter to show the results."<<endl;
				system("pause");
				b=0;
				break;
			}
			if(isFileInput&&(feof(f))){
				if(!normend){
					cout<<"Error:input file ends without -2, press enter to show the results."<<endl;
					system("pause");
					b=0;
					goto x;
				}
			}
		}
		if(b==0){
			break;
		}
		if(temp>24){
			cout<<"Error:number too large, press enter to show the results."<<endl;
			system("pause");
			b=0;
			break;
		}
		playing[temp]=1,isPlayed[temp]=1;
		temp=0;
x:		if(setbox()==-1){//执行setbox 
			b=0;
			break;
		}else{
			clear_playing();
		}
	}
	clear_playing();
	if(isFileInput){
		fclose(stdin);
		freopen("CON","r",stdin);
	}
	for(int i=0;i!=25;++i){//显示所有非空箱子的物品 
		if(isPlayed[i]){
			cout<<"box "<<i<<":";
			for(int j=0;(j!=54)&&(box[i][j]!=0);++j){
				if(j%9==0){
					cout<<endl;
				}
				cout.setf(ios::left);
				cout.width(4);
				cout<<box[i][j];
				//cout<<box[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
	}
	cout<<"Writing commands to file "<<name<<" ..."<<endl;
	if(mode){
		fout.open(name.c_str(),ios::out);
	}else{
		fout.open(name.c_str(),ios::app);
	}
	if(!fout.is_open()){
		cout<<"Error:fail to open the file."<<endl;
		fout.close();
		return 0;
	}
	for(int i=0;i!=25;++i){//文件覆写 
		if(isPlayed[i]){
			for(int j=0;(j!=54)&&(box[i][j]!=0);++j){
				fout<<"item replace block ^-"<<i<<" ^"<<(key[i]-1)<<" ^"<<((j<27)?3:4)<<" container."<<((j<27)?j:(j-27))
				<<" with minecraft:"<<((box[i][j]==-1)?"shears":"dirt")<<" "<<((box[i][j]==-1)?1:box[i][j])<<endl;
			}
		}
	}
	fout.close();
y:	cout<<"Successfully write the file. Do you want to change its name? Press any key to rename or just enter to skip."<<endl;
	getline(cin,choice);
	if(choice.size()>0){
		cout<<"Input the new name."<<endl;
		string newname;
		cin>>newname;
		newname+=".mcfunction";
		if(rename(name.c_str(),newname.c_str())){
			cout<<"Error:fail to rename the file."<<endl;
			goto y;
		}
		cout<<"Successfully rename the file."<<endl;
	}
	system("pause");
	return 0;
}
