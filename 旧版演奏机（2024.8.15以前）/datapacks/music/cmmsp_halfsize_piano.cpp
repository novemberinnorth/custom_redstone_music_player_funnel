#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
using namespace std;
const string default_name="setnote.mcfunction";//默认文件名 
//const int key[25]={1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1};//1为黑键，0为白键
const string block_name[27]={"glass","white_stained_glass","orange_stained_glass","magenta_stained_glass",
"light_blue_stained_glass","yellow_stained_glass","lime_stained_glass","pink_stained_glass","gray_stained_glass",
"light_gray_stained_glass","cyan_stained_glass","purple_stained_glass","blue_stained_glass","brown_stained_glass",
"white_wool","orange_wool","magenta_wool","light_blue_wool","yellow_wool","lime_wool","pink_wool","gray_wool",
"light_gray_wool","cyan_wool","purple_wool","blue_wool","brown_wool"};//代表空格的不同方块 
int box[4][25][54]={{{0}}};//保存所有箱子的内容 
//int isPlayed[25]={0};//某个音符是否演奏过 
int currentp[4][25]={{0}};//当前箱子存放序列的末端 
int playing[25]={0};//当前要同时演奏的音符
bool isFileInput=0;//是否为从文件输入 
bool isAdded=0;//是否步进到下一个空格名（0代表加过） 
int velocity=1,v=0;//保存节拍和当前正在演奏的播放器编号 
string line;//输入的一行 
string name;//文件名
string input;//读取的文件名 
string choice;//用于保存用户选择 
fstream fout;
FILE* f;
void initialize(){//初始化所有变量数组 
	for(int k=0;k!=4;++k){
		for(int i=0;i!=25;++i){
			//isPlayed[i]=0;
			currentp[k][i]=0,playing[i]=0;
			for(int j=0;j!=54;++j){
				box[k][i][j]=0;
			}
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
			if(box[v][i][currentp[v][i]]!=0){
				++currentp[v][i];
				if(currentp[v][i]==54){
					cout<<"Error:box is full, press enter to show the results."<<endl;
					system("pause");
					return -1;
				} 
			}
			box[v][i][currentp[v][i]]=-1;
			++currentp[v][i];
			if(currentp[v][i]==54){
				cout<<"Error:box is full, press enter to show the results."<<endl;
				system("pause");
				return -1;
			} 
		}else{//否则，在当前位置加一个方块，若已满64个，则先步进到下一格 
			if(box[v][i][currentp[v][i]]==64){
				++currentp[v][i];
				if(currentp[v][i]==54){
					cout<<"Error:box is full, press enter to show the results."<<endl;
					system("pause");
					return -1;
				} 
			}
			++box[v][i][currentp[v][i]];
		}
	}
	return 0;
}
int main(){
st:	initialize();
	string svel;
	cout<<"Input velocity, with 2 for double and 4 for four times original speed (press enter to use 1 as default) ."<<endl;
	getline(cin,svel);
	if(svel.size()!=0){
		velocity=svel[0]-'0';
	}
	if(velocity>4 || velocity<1){
		cout<<"Current velocity is unavaliable. Please enter 1, 2, 3, 4 only."<<endl;
		system("pause");
		goto st;
	}
	//cout<<velocity<<endl;
//	if(velocity!=2&&velocity!=3){
//		velocity=1;
//	}
	cout<<"Input file's name without suffix (press enter to use the default name) ."<<endl;
	getline(cin,name);
	//getline(cin,name);
	if(name.size()==0){
		name=default_name;
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
				playing[temp]=1;//准备演奏temp对应的音符 
				//isPlayed[temp]=1;
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
		playing[temp]=1; 
		//isPlayed[temp]=1;
		temp=0;
x:		if(setbox()==-1){//执行setbox 
			b=0;
			break;
		}else{
			clear_playing();
		}
		++v;
		if(v==velocity){
			v=0;
		}
	}
	clear_playing();
	if(isFileInput){
		fclose(stdin);
		freopen("CON","r",stdin);
	}
	for(int k=0;k!=velocity;++k){
		for(int i=0;i!=25;++i){//显示所有非空箱子的物品 
			if(box[k][i][0]!=0){
				cout<<"box "<<k<<"_"<<i<<":";
				for(int j=0;(j!=54)&&(box[k][i][j]!=0);++j){
					if(j%9==0){
						cout<<endl;
					}
					cout.setf(ios::left);
					cout.width(4);
					cout<<box[k][i][j];
					//cout<<box[i][j]<<" ";
				}
				cout<<endl;
			}
			cout<<endl;
		}
		cout<<endl;
	}
	for(int k=0;k!=velocity;++k){
		cout<<"Writing commands to file "<<name<<"_v"<<velocity<<"_"<<(k+1)<<" ..."<<endl;
		if(mode){
			fout.open((name+"_v"+to_string(velocity)+"_"+to_string(k+1)+".mcfunction").c_str(),ios::out);
		}else{
			fout.open((name+"_v"+to_string(velocity)+"_"+to_string(k+1)+".mcfunction").c_str(),ios::app);
		}
		if(!fout.is_open()){
			cout<<"Error:fail to open the file."<<endl;
			fout.close();
			return 0;
		}
		for(int i=0;i!=25;++i){//文件覆写 
			if(box[k][i][0]!=0){
				int temp=0;
				for(int j=0;(j!=54)&&(box[k][i][j]!=0);++j){
					if((box[k][i][j]==-1)&&isAdded){
						isAdded=0;
						++temp;
					}else if(box[k][i][j]!=-1){
						isAdded=1;
					}
					if(i<=12){
						fout<<"item replace block ^-"<<(i+1)<<" ^-2 ^"<<((j<27)?5:6)<<" container."<<((j<27)?j:(j-27))
						<<" with minecraft:"<<((box[k][i][j]==-1)?"shears":block_name[temp])<<" "<<((box[k][i][j]==-1)?1:box[k][i][j])<<endl;
					}
					if(i>=12){
						fout<<"item replace block ^-"<<(i-11)<<" ^2 ^"<<((j<27)?3:4)<<" container."<<((j<27)?j:(j-27))
						<<" with minecraft:"<<((box[k][i][j]==-1)?"shears":block_name[temp])<<" "<<((box[k][i][j]==-1)?1:box[k][i][j])<<endl;
					}
				}
			} 
		}	
		fout.close();
y:		cout<<"Successfully write the file. Do you want to change its name? Press any key to rename or just enter to skip."<<endl;
		getline(cin,choice);
		if(choice.size()>0){
			cout<<"Input the new name."<<endl;
			string newname;
			cin>>newname;
			newname+=".mcfunction";
			if(rename((name+"_"+to_string(k+1)+".mcfunction").c_str(),newname.c_str())){
				cout<<"Error:fail to rename the file."<<endl;
				goto y;
			}
			cout<<"Successfully rename the file."<<endl;
		}
	}
	system("pause");
	return 0;
}
