//custom redstone music player funnel shulkerbox v1.0
//本程序基于Java版Minecraft 1.17的游戏性质及在游戏中搭建的自定义红石音乐演奏机而编写
//本程序适配以下建筑：crmpf_arc_shulkerbox_w1.0.nbt
//使用前，请确保通过视频等方式了解其工作原理
//更多信息请关注 https://github.com/novemberinnorth/custom_redstone_music_player_funnel
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

//以下为类定义区
class Shulkerbox{//潜影盒类
    public:
        Shulkerbox(){
            for(int i = 0; i != 27; ++i){
                storage[i] = 0;
            }
        }
        Shulkerbox(int array[27]){
            for(int i = 0; i != 27; ++i){
                storage[i] = array[i];
            }
        }
        void show(std::string s);
        void write_function(int j, int temp1, int temp2, bool isFunnel);
    private:
        int storage[27];
};

//以下为全局常量定义区
const std::string STACKABLE_ITEM_NAME[14] = {"white_wool", "orange_wool", "magenta_wool", "light_blue_wool", "yellow_wool", "lime_wool", "pink_wool", "gray_wool", 
"light_gray_wool", "cyan_wool", "purple_wool", "blue_wool", "brown_wool", "cobblestone"};//可堆叠的物品名称
const std::string NON_STACKABLE_ITEM_NAME = "shears";//不可堆叠的物品名称
const std::string DEFAULT_FILE_NAME = "setnote.mcfunction";//默认的输出文件名
const int MAX_VELOCITY = 8;//最大倍速，请确保建有配套的适用于多倍速演奏的建筑

//以下为全局变量定义区
int temp_shulkerbox_column[MAX_VELOCITY][25][27] = {{{0}}};//临时的潜影盒列，从乐谱读取的信息经处理后存在此处，当有潜影盒装满或读取结束时打包装进shulkerbox_array中
int tsc_position[MAX_VELOCITY][25] = {{0}};//记录每个临时潜影盒最后一个有物品的格子的位置
int playnote[25] = {0};//从line中读取当前哪些音符要演奏，1为演奏，0为不演奏
int velocity = 1, v = 0;//记录用户自定义的倍速和辅助变量
int mode = 1;//输出文件模式，1为覆盖文件，0为追加文件
int count_line = 0;//计算当前读到的行数，便于提供报错信息
int count_shulkerbox_num[MAX_VELOCITY] = {0};//统计每个节拍演奏机单片存储系统的潜影盒总数
int temp = 0;//临时变量
int package_a, package_b, package_c, package_d, package_dt, package_count;//用于Package函数的各个变量，分别表示需要单片存储系统含有的大型箱子-漏斗组的数量、除去完整组合剩下的所需容器数量、存储系统尾端容器所需的格子数、存储系统所有容器的个数、package_d的副本、当前容器装到第几个格子
bool isFileInput = 0;//是否为从文件读入乐谱，1为是，0为否
bool endWhile = 0;//是否结束读取乐谱的while循环，1为结束，0为继续
bool isNormEnd = 0;//读取乐谱是否正常结束，1为正常，0为非正常
bool isFunnel = 0;//容器是否为漏斗，1为漏斗，0为大型箱子
std::string line;//直接从乐谱读取的一行字符串
std::string stemp;//临时字符串变量
std::string input_file_name;//存有乐谱的文件名，一般为.txt格式，输入时要带后缀
std::string output_file_name;//存有输出的函数指令的文件名，一般为.mcfunction格式，输入时不带后缀
std::string custom_choice;//判断用户是否想从文件读取乐谱，长度大于0即为从文件读入乐谱
std::string blank = "            ";//用于Shulkerbox.write_function()中的格式化输出
std::fstream fout;//用于写入函数命令的输出文件流
FILE* f;//用于从文件读入乐谱的文件指针
std::vector<Shulkerbox> shulkerbox_array[MAX_VELOCITY][25];//按朴素的乐谱顺序存放的潜影盒队列
std::vector<Shulkerbox> correct_shulkerbox_array[MAX_VELOCITY][25];//经Package函数调整后，按装箱坐标重新存放的潜影盒队列
std::vector<int> coordinator[MAX_VELOCITY][2];//装箱坐标，第一行0~(package_d-1)表示潜影盒装在第几个容器，奇数为漏斗，偶数为大型箱子；第二行表示潜影盒具体装在容器中的第几个格子，实践中不常用

//以下为函数区
void Shulkerbox::show(std::string s){//按格式打印storage中的物品
    for(int i = 0; i != 3; ++i){
        std::cout<<s;
        for(int j = 0; j != 9; ++j){
            std::cout<<storage[i * 9 + j]<<"   ";
        }
        std::cout<<std::endl;
    }
}
void Shulkerbox::write_function(int j, int temp1, int temp2, bool isFunnel){//按Java版mc 1.17的风格生成函数命令，若其他版本在命令层面有变动，请自行对照修改
    fout<<"item replace block ^-"<<(j + 1)<<" ^"<<(2 + temp1 / 2)<<" ^"
    <<((temp1 / 2 % 2) ? (isFunnel ? 4 : (temp2 < 27 ? 2 : 3)) : (isFunnel ? 2 : (temp2 < 27 ? 3 : 4)))//三维坐标，与用户执行命令的位置及存储系统强相关
    <<" container."<<(temp2 < 27 ? temp2 : (temp2 - 27))//大型箱子在执行时看作两个小型箱子
    <<" with minecraft:shulker_box{BlockEntityTag:{Items:[";
    int tempint = 0;
    for(int i = 0; i != 27; ++i){
        if(storage[i] == -1){//此处放置不可堆叠物品
            fout<<"{id:\"minecraft:"<<NON_STACKABLE_ITEM_NAME<<"\",Count:1b,Slot:"<<i<<"b}";
        }else{//此处放置可堆叠物品
            fout<<"{id:\"minecraft:"<<STACKABLE_ITEM_NAME[tempint]<<"\",Count:"<<storage[i]<<"b,Slot:"<<i<<"b}";
            if(i < 26 && storage[i + 1] == -1){//仅当下一个格子是不可堆叠物品时才会步进到下一个可堆叠物品的名字
                ++tempint;
            }
        }
        if(i < 26){
            fout<<",";
        }
    }
    fout<<"]}}"<<std::endl;
}
void Clearall(){//清空所有变量数组
    for(int i = 0; i != MAX_VELOCITY; ++i){
        coordinator[i][0].clear();
        coordinator[i][0].shrink_to_fit();
        coordinator[i][1].clear();
        coordinator[i][1].shrink_to_fit();
        count_shulkerbox_num[i] = 0;
        for(int j = 0; j != 25; ++j){
            tsc_position[i][j] = 0;
            playnote[j] = 0;
            shulkerbox_array[i][j].clear();
            shulkerbox_array[i][j].shrink_to_fit();
            for(int k = 0; k != 27; ++k){
                temp_shulkerbox_column[i][j][k] = 0;
            }
        }
    }
}
void Clear_playnote(){//清空playnote，准备下一行输入 
	for(int i = 0; i != 25; ++i){
		playnote[i]=0;
	}
}
void Clear_tsc(int v){//清空指定节拍的临时潜影盒列
    for(int i = 0; i != 25; ++i){
        tsc_position[v][i] = 0;
        for(int j = 0; j != 27; ++j){
            temp_shulkerbox_column[v][i][j] = 0;
        }
    }
}
void Package(){
    //根据count_shulkerbox_num的数据生成装箱坐标
    for(int i = 0; i != velocity; ++i){
        //计算各个变量的值
        package_a = count_shulkerbox_num[i] / 59;//需要单片存储系统含有的大型箱子-漏斗组的数量
        package_b = (count_shulkerbox_num[i] % 59 == 0) ? 0 : (count_shulkerbox_num[i] % 59 > 54 ? 2 : 1);//除去完整组合剩下的所需容器数量
        package_c = count_shulkerbox_num[i] % 59 - (count_shulkerbox_num[i] % 59 > 54 ? 54 : 0);//存储系统尾端容器所需的格子数
        package_d = 2 * package_a + package_b;//存储系统所有容器的个数
        package_dt = package_d;//package_d的副本
        package_count = 1;//当前容器装到第几个格子
        for(int j = 0; j != count_shulkerbox_num[i]; ++j){//基础逻辑：每个容器先输出自己的第一格物品，随后第一格被后续容器占据，仅当后续容器输出完后才会输出自己其他格的物品
            if(j < package_d){//先按顺序生成所有容器的第1格
                coordinator[i][0].push_back(j);
                coordinator[i][1].push_back(0);
            }else{//再按倒序生成容器的其它格
                if(package_dt % 2 == 1){//大型箱子
                    if((package_dt == package_d && package_count == package_c) || package_count == 54){//若该容器已装满，或当该容器为存储系统尾端容器时，已装到package_c所示的格子数，则应转到下一个容器
                        --package_dt;
                        --j;//这一步为切换容器，故不应计入正常步数
                        package_count = 1;
                    }else{//录入容器坐标
                        coordinator[i][0].push_back(package_dt - 1);
                        coordinator[i][1].push_back(package_count);
                        ++package_count;
                    }
                }else if(package_dt % 2 == 0){//漏斗
                    if((package_dt == package_d && package_count == package_c) || package_count == 5){//若该容器已装满，或当该容器为存储系统尾端容器时，已装到package_c所示的格子数，则应转到下一个容器
                        --package_dt;
                        --j;//这一步为切换容器，故不应计入正常步数
                        package_count = 1;
                    }else{//录入容器坐标
                        coordinator[i][0].push_back(package_dt - 1);
                        coordinator[i][1].push_back(package_count);
                        ++package_count;
                    }
                }
            }
        }
        //根据装箱坐标把潜影盒按正确的顺序装进correct_shulkerbox_array里
        for(int j = 0; j != 25; ++j){
            for(int k = 0; k != package_d; ++k){
                for(int l = 0; l != count_shulkerbox_num[i]; ++l){
                    if(coordinator[i][0][l] == k){
                        correct_shulkerbox_array[i][j].push_back(shulkerbox_array[i][j][l]);
                    }
                }
            }
        }
    }
}
void Package_tsc(int v){//将指定节拍的临时潜影盒列打包装进shulkerbox_array里
    for(int i = 0; i != 25; ++i){
        Shulkerbox temp_shulkerbox(temp_shulkerbox_column[v][i]);
        shulkerbox_array[v][i].push_back(temp_shulkerbox);
    }
    ++count_shulkerbox_num[v];
}
void Setbox(){//-1代表不可堆叠的物品，0~64代表可堆叠的物品数量
    for(int i = 0; i != 25; ++i){//预检查，若当前节拍的临时潜影盒列无法装下playnote的内容（潜影盒已装到最后一格，且当前音符要演奏，或当前音符不用演奏但最后一格已装满64个可堆叠物品或装有不可堆叠物品），则打包并清理
        if(tsc_position[v][i] == 26 && (playnote[i] || ((!playnote[i]) && (temp_shulkerbox_column[v][i][26] == 64 || temp_shulkerbox_column[v][i][26] == -1)))){
            Package_tsc(v);
            Clear_tsc(v);
            break;//只要有一个潜影盒装不下就完成检查
        }
    }
    for(int i = 0; i != 25; ++i){
        if(playnote[i]){//当前音符要演奏
            ++tsc_position[v][i];//步进到下一格
            temp_shulkerbox_column[v][i][tsc_position[v][i]] = -1;
        }else{//当前音符不演奏
            if(temp_shulkerbox_column[v][i][tsc_position[v][i]] == 64 || temp_shulkerbox_column[v][i][tsc_position[v][i]] == -1){//若当前这格已装满64个可堆叠物品或装有不可堆叠物品就步进到下一格
                ++tsc_position[v][i];
            }
            ++temp_shulkerbox_column[v][i][tsc_position[v][i]];
        }
    }
}
int main(){
    Clearall();
    std::cout<<"crmpf_shulkerbox_v1.0"<<"   如发现问题，请至 https://github.com/novemberinnorth/custom_redstone_music_player_funnel/issues 或向 citcra@foxmail.com 反馈。\n"<<std::endl;
    //输入用户自定义数据
    std::cout<<"输入速度倍数（1~8倍），只接受数字1~8，按enter默认为1倍速。"<<std::endl;
    std::cin>>velocity;
    if(velocity < 2 || velocity > 8){
        velocity = 1;
    }
    std::cout<<"输入自定义的输出文件名称，不需要带后缀，按enter为默认名称。"<<std::endl;
    std::getline(std::cin, output_file_name);
    if(output_file_name.size() == 0){
        output_file_name = DEFAULT_FILE_NAME;
    }
    std::cout<<"输入自定义输出模式，c为覆盖原文件，a为在原文件后加入新内容，按enter默认为覆盖原文件。"<<std::endl;
    std::getline(std::cin, stemp);
    if((stemp.size() == 0) || (stemp == "c") || (stemp == "C")){
        mode = 1;
    }else{
        mode = 0;
    }
z:  std::cout<<"乐谱规则：\n"
             <<"    1. 每一行为一个最小时值，具体时值由游戏刻与倍速决定\n"
             <<"    2. 每一行可以容纳多个音符，用空格隔开\n"
             <<"    3. 0~24代表#F(8-) ~ #F(8+)\n"
             <<"    4. -1表示一个最小时值的休止符\n"
             <<"    5. -2表示乐谱结束，停止读取乐谱"<<std::endl;
    std::cout<<"你想从文件里读取乐谱吗？按其他任意键以输入文件名或按enter以继续从键盘输入。"<<std::endl;
    std::getline(std::cin, custom_choice);
    if(custom_choice.size() > 0){
        isFileInput = 1;
        std::cout<<"请确保你的文件格式符合上述要求。输入你的文件名（带后缀）。"<<std::endl;
        std::cin>>input_file_name;
        f = freopen(input_file_name.c_str(), "r", stdin);
        if(f == NULL){
            std::cout<<"错误：无法打开输入文件。"<<std::endl;
            goto z;
        }
    }
    //读取乐谱
    while(!endWhile){
        std::getline(std::cin, line);//从乐谱读一行存到line中
        ++count_line;
        for(auto i = 0; i != line.size(); ++i){//按字符读取line的内容
            if((line[i] - '0' >= 0) && (line[i] - '0' <= 9)){//两位数的字符-数字转换
                if(temp == 0){
                    temp = line[i] - '0';
                }else{
                    temp = temp * 10 + line[i] - '0';
                }
            }else if(line[i] == ' '){//空格表示一个数字读取完毕
                if(temp > 24){
                    std::cout<<"在第"<<count_line<<"行发现错误：一个数字大于24。读取意外终止，按enter显示结果。"<<std::endl;
                    system("pause");
                    endWhile = 1;
                    break;
                }
                playnote[temp] = 1;//将信息存到playnote中
                temp = 0;
            }else if(line[i] == '-'){//负号说明接下来为状态参量
                if((i+1) != line.size()){
                    if(line[i+1] == '1'){//当前为休止符，直接进入Setbox环节
                        goto x;
                    }else if(line[i+1] == '2'){//正常结束
                        endWhile = 1;
                        isNormEnd = 1;
                        break;
                    }else{
                        std::cout<<"在第"<<count_line<<"行发现错误：负号后数字非法。读取意外终止，按enter显示结果。"<<std::endl;
                        system("pause");
                        endWhile = 1;
                        break;
                    }
                }else{
                    std::cout<<"在第"<<count_line<<"行发现错误：负号后没有数字。读取意外终止，按enter显示结果。"<<std::endl;
                    system("pause");
                    endWhile = 1;
                    break;
                }
            }else{
                std::cout<<"在第"<<count_line<<"行发现错误：检测到非法输入。读取意外终止，按enter显示结果。"<<std::endl;
                system("pause");
                endWhile = 1;
                break;
            }
            if(isFileInput && feof(f) && !isNormEnd){
                std::cout<<"在第"<<count_line<<"行发现错误：文件未以-2结尾。读取意外终止，按enter显示结果。"<<std::endl;
                system("pause");
                endWhile = 1;
                goto x;
            }
        }
        if(endWhile){
            break;
        }
        //将temp中剩余的数字读完
        if(temp > 24){
            std::cout<<"在第"<<count_line<<"行发现错误：一个数字大于24。读取意外终止，按enter显示结果。"<<std::endl;
            system("pause");
            endWhile = 1;
            break;
        }
        playnote[temp] = 1;
        temp = 0;
        //playnote的信息填写完毕，进入Setbox环节
x:      Setbox();
        Clear_playnote();
        //步进一拍
        ++v;
        if(v == velocity){
            v = 0;
        }
    }
    //乐谱读取结束，把临时潜影盒列剩余的物品全部打包清理
    for(int i = 0; i != velocity; ++i){
        Package_tsc(i);
        Clear_tsc(i);
    }
    //进入Package环节
    std::cout<<"正在装箱..."<<std::endl;
    Package();
    //打印装箱内容
    if(isFileInput){
        fclose(stdin);
        freopen("CON","r",stdin);
    }
    std::cout<<"打印装箱内容...\n"
             <<"注意：-1代表不可堆叠的物品，0~64代表可堆叠的物品数量。\n"<<std::endl;
    for(int i = 0; i != velocity; ++i){
        std::cout<<"--------------------\n"<<std::endl;
        std::cout<<"节拍"<<i<<"演奏机内容：\n"<<std::endl;
        for(int j = 0; j != 25; ++j){
            std::cout<<"   序号"<<j<<"单片演奏机存储系统：\n"<<std::endl;
            int temp1 = 0, temp2 = 0;
            std::cout<<"      容器"<<temp1<<(isFunnel ? "（漏斗）" : "（大型箱子）")<<"：\n"<<std::endl;
            for(int k = 0; k != count_shulkerbox_num[i]; ++k){
                if((!isFunnel && temp2 == 54) || (isFunnel && temp2 == 5)){
                    temp2 = 0;
                    ++temp1;
                    isFunnel = (!isFunnel);
                    std::cout<<"      容器"<<temp1<<(isFunnel ? "（漏斗）" : "（大型箱子）")<<"：\n"<<std::endl;
                }
                std::cout<<"         第"<<(temp2 + 1)<<"格潜影盒内容："<<std::endl;
                correct_shulkerbox_array[i][j][k].show(blank);
            }
        }
    }
    isFunnel = 0;
    //填写函数命令
    for(int i = 0; i != velocity; ++i){
        //输出文件命名格式：output_file_name_(当前节拍)in(用户自定义倍速).mcfunction
        std::cout<<"正在将命令写入文件"<<output_file_name<<"_"<<(i + 1)<<"in"<<velocity<<"...";
        if(mode){
            fout.open((output_file_name + "_" + std::to_string(i + 1) + "in" + std::to_string(velocity) + ".mcfunction").c_str(), std::ios::out);
        }else{
            fout.open((output_file_name + "_" + std::to_string(i + 1) + "in" + std::to_string(velocity) + ".mcfunction").c_str(), std::ios::app);
        }
        if(!fout.is_open()){
            std::cout<<"\n错误：无法打开文件，程序自动关闭。"<<std::endl;
            fout.close();
            return 0;
        }
        for(int j = 0; j != 25; ++j){
            int temp1 = 0, temp2 = 0;
            for(int k = 0; k != count_shulkerbox_num[i]; ++k){
                if((!isFunnel && temp2 == 54) || (isFunnel && temp2 == 5)){
                    temp2 = 0;
                    ++temp1;
                    isFunnel = (!isFunnel);
                }
                correct_shulkerbox_array[i][j][k].write_function(j, temp1, temp2, isFunnel);
            }
        }
        std::cout<<"完成。"<<std::endl;
    }
    fout.close();
    std::cout<<"文件写入完成，按任意键关闭程序。"<<std::endl;
    system("pause");
    return 0;
}