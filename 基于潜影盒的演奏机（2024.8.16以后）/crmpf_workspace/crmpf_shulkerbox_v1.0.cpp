//custom redstone music player funnel shulkerbox v1.0
//本程序基于Java版Minecraft 1.17的游戏性质及在游戏中搭建的自定义红石音乐演奏机而编写
//本程序适配以下建筑：crmpf_arc_shulkerbox_w1.0.nbt  crmpf_arc_shulkerbox_w1.0.litematic  crmpf_arc_shulkerbox_w1.0_quadro.litematic
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
            deadbox_type = 0;
        }
        Shulkerbox(int array[27]){
            for(int i = 0; i != 27; ++i){
                storage[i] = array[i];
            }
            deadbox_checker();
        }
        int deadbox_type = -1;//是否为死盒及死盒类型，-1为非死盒，0为零物品盒，1为单物品盒
        void show(std::string s);
        void write_function(int j, int temp1, int temp2, bool isFunnel);
        void deadbox_checker();
    private:
        int storage[27];
};

//以下为全局常量定义区
const std::string STACKABLE_ITEM_NAME[14] = {"white_wool", "orange_wool", "magenta_wool", "light_blue_wool", "yellow_wool", "lime_wool", "pink_wool", "gray_wool", 
"light_gray_wool", "cyan_wool", "purple_wool", "blue_wool", "brown_wool", "cobblestone"};//可堆叠的物品名称
const std::string NON_STACKABLE_ITEM_NAME = "shears";//不可堆叠的物品名称
const std::string DEFAULT_FILE_NAME = "setnote.mcfunction";//默认的输出文件名
const std::string PITCH[25] = {"F#", "G", "A♭", "A", "B♭", "B", "C", "D♭", "D", "E♭", "E", "F", "F#", "G", "A♭", "A", "B♭", "B", "C", "D♭", "D", "E♭", "E", "F", "F#"};//音高记号
const std::string VERSION_SUFFIX = "_v1.0";//版本号后缀，用于区分不同版本的代码输出的文件
const int MAX_VELOCITY = 8;//最大倍速，请确保建有配套的适用于多倍速演奏的建筑

//以下为全局变量定义区
int temp_array[27] = {0};//临时数组，记录剩余的数组装回temp_shulkerbox_column
int temp_shulkerbox_column[MAX_VELOCITY][25][27] = {{{0}}};//临时的潜影盒列，从乐谱读取的信息经处理后存在此处，当有潜影盒装满或读取结束时打包装进shulkerbox_array中
int tsc_position[MAX_VELOCITY][25] = {{0}};//记录每个临时潜影盒最后一个有物品的格子的位置
int playnote[25] = {0};//从line中读取当前哪些音符要演奏，1为演奏，0为不演奏
int velocity = 1, v = 0;//记录用户自定义的倍速和辅助变量
int mode = 1;//输出文件模式，1为覆盖文件，0为追加文件
int count_line = 0;//计算当前读到的行数，便于提供报错信息
int count_shulkerbox_num[MAX_VELOCITY][25] = {{0}};//统计每个节拍演奏机单片存储系统的潜影盒总数
int temp = 0;//临时变量
int delay_begin = 0, db = 0;//在乐谱开头引入多少行延迟
int package_a, package_b, package_c, package_d, package_dt, package_count;//用于Package函数的各个变量，分别表示需要单片存储系统含有的大型箱子-漏斗组的数量、除去完整组合剩下的所需容器数量、存储系统尾端容器所需的格子数、存储系统所有容器的个数、package_d的副本、当前容器装到第几个格子
bool isFileInput = 0;//是否为从文件读入乐谱，1为是，0为否
bool endWhile = 0;//是否结束读取乐谱的while循环，1为结束，0为继续
bool isNormEnd = 0;//读取乐谱是否正常结束，1为正常，0为非正常
bool isFunnel = 0;//容器是否为漏斗，1为漏斗，0为大型箱子
bool isDeadbox = 0;//是否出现死盒，1为是，0为否
std::string line;//直接从乐谱读取的一行字符串
std::string stemp;//临时字符串变量
std::string input_file_name;//存有乐谱的文件名，一般为.txt格式，输入时要带后缀
std::string output_file_name;//存有输出的函数指令的文件名，一般为.mcfunction格式，输入时不带后缀
std::string custom_choice;//判断用户是否想从文件读取乐谱，长度大于0即为从文件读入乐谱
std::string blank = "                ";//用于格式化输出
std::fstream fin;//用于从文件读入乐谱的文件流
std::fstream fout;//用于写入函数命令的文件流
std::vector<Shulkerbox> shulkerbox_array[MAX_VELOCITY][25];//按朴素的乐谱顺序存放的潜影盒队列
std::vector<Shulkerbox> correct_shulkerbox_array[MAX_VELOCITY][25];//经Package函数调整后，按装箱坐标重新存放的潜影盒队列
std::vector<int> coordinator[MAX_VELOCITY][25][2];//装箱坐标，第一行0~(package_d - 1)表示潜影盒装在第几个容器，奇数为漏斗，偶数为大型箱子；第二行表示潜影盒具体装在容器中的第几个格子，实践中不常用

//以下为函数区
void Shulkerbox::show(std::string s){//按格式打印storage中的物品
    for(int i = 0; i != 3; ++i){
        fout << s;
        for(int j = 0; j != 9; ++j){
            fout << storage[i * 9 + j] << "   ";
        }
        fout << std::endl;
    }
    fout << std::endl;
}
void Shulkerbox::write_function(int j, int temp1, int temp2, bool isFunnel){//按Java版mc 1.17的风格生成函数命令，若其他版本在命令层面有变动，请自行对照修改
    fout  <<  "item replace block ^-" << (j + 1) << " ^" << (2 + temp1 / 2) << " ^"
    << ((temp1 / 2 % 2) ? (isFunnel ? 4 : (temp2 < 27 ? 2 : 3)) : (isFunnel ? 2 : (temp2 < 27 ? 3 : 4)))//三维坐标，与用户执行命令的位置及存储系统强相关
    << " container." << (temp2 < 27 ? temp2 : (temp2 - 27))//大型箱子在执行时看作两个小型箱子
    << " with minecraft:shulker_box{BlockEntityTag:{Items:[";
    int tempint = 0;
    for(int i = 0; i != 27; ++i){
        if(storage[i] == -1){//此处放置不可堆叠物品
            fout << "{id:\"minecraft:" << NON_STACKABLE_ITEM_NAME << "\",Count:1b,Slot:" << i << "b}";
        }else{//此处放置可堆叠物品
            fout << "{id:\"minecraft:" << STACKABLE_ITEM_NAME[tempint] << "\",Count:" << storage[i] << "b,Slot:" << i << "b}";
            if(i < 26 && storage[i + 1] == -1){//仅当下一个格子是不可堆叠物品时才会步进到下一个可堆叠物品的名字
                ++tempint;
            }
        }
        if(i < 26){
            fout << ",";
        }
    }
    fout << "]}}" << std::endl;
}
void Shulkerbox::deadbox_checker(){//检查潜影盒是否为死盒及死盒的类型
    for(int i = 26; i >= 0; --i){
        if(i != 0 && storage[i] != 0){//只要除第一格外其他任意格有物品，就不是死盒
            deadbox_type = -1;
            break;
        }else if(i == 0 && storage[i] == 0){//零物品盒
            deadbox_type = 0;
        }else if(i == 0 && (storage[i] == 1 || storage[i] == -1)){//单物品盒
            deadbox_type = 1;
        }
    }
}
void Clearall(){//清空所有变量数组
    for(int i = 0; i != MAX_VELOCITY; ++i){
        for(int j = 0; j != 25; ++j){
            coordinator[i][j][0].clear();
            coordinator[i][j][0].shrink_to_fit();
            coordinator[i][j][1].clear();
            coordinator[i][j][1].shrink_to_fit();
            count_shulkerbox_num[i][j] = 0;
            tsc_position[i][j] = 0;
            playnote[j] = 0;
            shulkerbox_array[i][j].clear();
            shulkerbox_array[i][j].shrink_to_fit();
            for(int k = 0; k != 27; ++k){
                temp_shulkerbox_column[i][j][k] = 0;
                temp_array[k] = 0;
            }
        }
    }
}
void Clear_array(){//清空临时数组
    for(int i = 0; i != 27; ++i){
        temp_array[i] = 0;
    }
}
void Clear_playnote(){//清空playnote，准备下一行输入 
	for(int i = 0; i != 25; ++i){
		playnote[i] = 0;
	}
}
void Clear_tsc(int v, int i){//清空指定节拍、指定列的临时潜影盒列
    tsc_position[v][i] = 0;
    for(int j = 0; j != 27; ++j){
        temp_shulkerbox_column[v][i][j] = 0;
    }
}
void Package(){
    //根据count_shulkerbox_num的数据生成装箱坐标
    for(int i = 0; i != velocity; ++i){
        for(int j = 0; j != 25; ++j){
            //计算各个变量的值
            package_a = count_shulkerbox_num[i][j] / 59;//需要单片存储系统含有的大型箱子-漏斗组的数量
            package_b = (count_shulkerbox_num[i][j] % 59 == 0) ? 0 : (count_shulkerbox_num[i][j] % 59 > 54 ? 2 : 1);//除去完整组合剩下的所需容器数量
            package_c = count_shulkerbox_num[i][j] % 59 - (count_shulkerbox_num[i][j] % 59 > 54 ? 54 : 0);//存储系统尾端容器所需的格子数
            package_d = 2 * package_a + package_b;//存储系统所有容器的个数
            package_dt = package_d;//package_d的副本
            package_count = 1;//当前容器装到第几个格子
            for(int k = 0; k != count_shulkerbox_num[i][j]; ++k){//基础逻辑：每个容器先输出自己的第一格物品，随后第一格被后续容器占据，仅当后续容器输出完后才会输出自己其他格的物品
                if(k < package_d){//先按顺序生成所有容器的第1格
                    coordinator[i][j][0].push_back(k);
                    coordinator[i][j][1].push_back(0);
                }else{//再按倒序生成容器的其它格
                    if(package_dt % 2 == 1){//大型箱子
                        if((package_dt == package_d && package_count == package_c) || package_count == 54){//若该容器已装满，或当该容器为存储系统尾端容器时，已装到package_c所示的格子数，则应转到下一个容器
                            --package_dt;
                            --k;//这一步为切换容器，故不应计入正常步数
                            package_count = 1;
                        }else{//录入容器坐标
                            coordinator[i][j][0].push_back(package_dt - 1);
                            coordinator[i][j][1].push_back(package_count);
                            ++package_count;
                        }
                    }else if(package_dt % 2 == 0){//漏斗
                        if((package_dt == package_d && package_count == package_c) || package_count == 5){//若该容器已装满，或当该容器为存储系统尾端容器时，已装到package_c所示的格子数，则应转到下一个容器
                            --package_dt;
                            --k;//这一步为切换容器，故不应计入正常步数
                            package_count = 1;
                        }else{//录入容器坐标
                            coordinator[i][j][0].push_back(package_dt - 1);
                            coordinator[i][j][1].push_back(package_count);
                            ++package_count;
                        }
                    }
                }
            }
            //根据装箱坐标把潜影盒按正确的顺序装进correct_shulkerbox_array里
            for(int k = 0; k != package_d; ++k){
                for(int l = 0; l != count_shulkerbox_num[i][j]; ++l){
                    if(coordinator[i][j][0][l] == k){
                        correct_shulkerbox_array[i][j].push_back(shulkerbox_array[i][j][l]);
                    }
                }
            }
        }
    }
}
void Package_tsc(int v, int i){//将指定节拍、指定列的临时潜影盒打包装进shulkerbox_array里
    Shulkerbox temp_shulkerbox(temp_shulkerbox_column[v][i]);
    temp_shulkerbox.deadbox_checker();
    if(temp_shulkerbox.deadbox_type != -1){//死盒
        if(temp_shulkerbox.deadbox_type == 0){
            std::cout << "警告：在演奏第" << count_line << "行时，位于第" << (v + 1) << "节拍 序号" << i << "（" << PITCH[i] << "）单片机发现死盒（零物品盒），将延迟两个时值重新演奏。" << std::endl;
            delay_begin += 2;
            isDeadbox = 1;
        }else if(temp_shulkerbox.deadbox_type == 1){
            std::cout << "警告：在演奏第" << count_line << "行时，位于第" << (v + 1) << "节拍 序号" << i << "（" << PITCH[i] << "）单片机发现死盒（单物品盒），将延迟一个时值重新演奏。" << std::endl;
            delay_begin += 1;
            isDeadbox = 1;
        }
    }else{//正常盒
        shulkerbox_array[v][i].push_back(temp_shulkerbox);
        ++count_shulkerbox_num[v][i];
    }
}
void Setbox(int v){//-1代表不可堆叠的物品，0~64代表可堆叠的物品数量
    for(int i = 0; i != 25; ++i){//预检查，若临时潜影盒列中有潜影盒无法装下playnote的内容（潜影盒已装到最后一格，且当前音符要演奏，或当前音符不用演奏但最后一格已装满64个可堆叠物品或装有不可堆叠物品），则对该潜影盒执行操作
        if(tsc_position[v][i] == 26 && (playnote[i] || ((!playnote[i]) && (temp_shulkerbox_column[v][i][26] == 64 || temp_shulkerbox_column[v][i][26] == -1)))){
            int j = 26;
            for( ; j >= 0; --j){//从后往前搜索整个箱子，找到最后一个装有剪刀的格子
                if(temp_shulkerbox_column[v][i][j] == -1){
                    break;
                }
            }
            if(j < 0){//没有找到剪刀，则打印警告信息
                std::cout << "警告：在演奏第" << count_line << "行时，位于第" << (v + 1) << "节拍 序号" << i << "（" << PITCH[i] << "）单片机由于距离上一次演奏的时长超出潜影盒容量限制，将会异常演奏一次。" << std::endl;
                Package_tsc(v, i);//直接打包成装满27格64个物品的潜影盒
                Clear_tsc(v, i);
            }else{
                temp_shulkerbox_column[v][i][j] = 0;//删除这格剪刀，其作用由潜影盒本身替代
                Clear_array();
                for(int k = j + 1; k < 27; ++k){
                    temp_array[k - j - 1] = temp_shulkerbox_column[v][i][k];//记录最后一格剪刀之后的数组，后面装回temp_shulkerbox_column
                    temp_shulkerbox_column[v][i][k] = 0;
                }
                Package_tsc(v, i);
                if(isDeadbox){
                    return;
                }
                tsc_position[v][i] = 0;
                for(int k = 0; k != 27; ++k){//利用temp_array初始化temp_shulkerbox_column和tsc_position
                    temp_shulkerbox_column[v][i][k] = temp_array[k];
                    if(temp_array[k] != 0){
                        tsc_position[v][i] = k;
                    }
                }
            }
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
    std::cout << "crmpf_shulkerbox_v1.0" << "   如发现问题，请至 https://github.com/novemberinnorth/custom_redstone_music_player_funnel/issues 或向 citcra@foxmail.com 反馈。\n" << std::endl;
    //输入用户自定义数据
    std::cout << "输入速度倍数（1~8倍），只接受数字1~8，其余输入默认为1倍速。" << std::endl;
    std::cin >> velocity;
    if(velocity < 2 || velocity > 8){
        velocity = 1;
    }
    std::cout << "输入自定义的输出文件名称，不需要带后缀，按enter为默认名称。" << std::endl;
    std::getline(std::cin, output_file_name);
    std::getline(std::cin, output_file_name);
    if(output_file_name.size() == 0){
        output_file_name = DEFAULT_FILE_NAME;
    }
    std::cout << "输入自定义输出模式，c为覆盖原文件，a为在原文件后加入新内容，按enter默认为覆盖原文件。" << std::endl;
    std::getline(std::cin, stemp);
    if((stemp.size() == 0) || (stemp == "c") || (stemp == "C")){
        mode = 1;
    }else{
        mode = 0;
    }
reinput:  std::cout << "乐谱规则：\n"
                    << "    1. 每一行为一个最小时值，具体时值由游戏刻与倍速决定\n"
                    << "    2. 每一行可以容纳多个音符，用空格隔开\n"
                    << "    3. 0~24代表#F(8-) ~ #F(8+)\n"
                    << "    4. -1表示一个最小时值的休止符\n"
                    << "    5. -2表示乐谱结束，停止读取乐谱" << std::endl;
    std::cout << "你想从文件里读取乐谱吗？按其他任意键以输入文件名或按enter以继续从键盘输入。" << std::endl;
    std::getline(std::cin, custom_choice);
    if(custom_choice.size() > 0){
        isFileInput = 1;
        std::cout << "请确保你的文件格式符合上述要求。输入你的文件名（带后缀），按enter默认为与输出文件同名的txt文件。" << std::endl;
        std::getline(std::cin, input_file_name);
        if(input_file_name.size() == 0){
            input_file_name = output_file_name + ".txt";
        }
        fin.open(input_file_name.c_str(), std::ios::in);
        if(!fin.is_open()){
            std::cout << "错误：无法打开输入文件。" << std::endl;
            goto reinput;
        }
    }
    do{//若出现死盒就会返回
        db = delay_begin;
        //读取乐谱
        while(!endWhile){
            if(isDeadbox){//出现死盒则更新起始延迟
                std::cout << "当前起始延迟：" << delay_begin << std::endl;
                isDeadbox = 0;
            }
            if(db > 0){//起始延迟相当于休止
                line = "-1";
                --db;
            }else{
                std::getline(fin, line);//从乐谱读一行存到line中
                ++count_line;
            }
            for(auto i = 0; i != line.size(); ++i){//按字符读取line的内容
                if((line[i] - '0' >= 0) && (line[i] - '0' <= 9)){//两位数的字符-数字转换
                    if(temp == 0){
                        temp = line[i] - '0';
                    }else{
                        temp = temp * 10 + line[i] - '0';
                    }
                }else if(line[i] == ' '){//空格表示一个数字读取完毕
                    if(temp > 24){
                        std::cout << "在第" << count_line << "行发现错误：一个数字大于24。读取意外终止，按enter显示结果。" << std::endl;
                        system("pause");
                        endWhile = 1;
                        break;
                    }
                    playnote[temp] = 1;//将信息存到playnote中
                    temp = 0;
                }else if(line[i] == '-'){//负号说明接下来为状态参量
                    if((i + 1) != line.size()){
                        if(line[i + 1] == '1'){//当前为休止符，直接进入Setbox环节
                            goto setbox;
                        }else if(line[i + 1] == '2'){//正常结束
                            endWhile = 1;
                            isNormEnd = 1;
                            break;
                        }else{
                            std::cout << "在第" << count_line << "行发现错误：负号后数字非法。读取意外终止，按enter显示结果。" << std::endl;
                            system("pause");
                            endWhile = 1;
                            break;
                        }
                    }else{
                        std::cout << "在第" << count_line << "行发现错误：负号后没有数字。读取意外终止，按enter显示结果。" << std::endl;
                        system("pause");
                        endWhile = 1;
                        break;
                    }
                }else{
                    std::cout << "在第" << count_line << "行发现错误：检测到非法输入。读取意外终止，按enter显示结果。" << std::endl;
                    system("pause");
                    endWhile = 1;
                    break;
                }
                if(isFileInput && fin.eof() && !isNormEnd){
                    std::cout << "在第" << count_line << "行发现错误：文件未以-2结尾。读取意外终止，按enter显示结果。" << std::endl;
                    system("pause");
                    endWhile = 1;
                    goto setbox;
                }
            }
            if(endWhile){
                break;
            }
            //将temp中剩余的数字读完
            if(temp > 24){
                std::cout << "在第" << count_line << "行发现错误：一个数字大于24。读取意外终止，按enter显示结果。" << std::endl;
                system("pause");
                endWhile = 1;
                break;
            }
            playnote[temp] = 1;
            temp = 0;
            //playnote的信息填写完毕，进入Setbox环节
setbox:     Setbox(v);
            if(isDeadbox){//发现死盒，则清零有关乐谱的所有变量，更新起始延迟并重新读取文件
                count_line = 0;
                v = 0;
                Clearall();
                if(isFileInput){
                    fin.close();
                    fin.open(input_file_name.c_str(), std::ios::in);
                }
                goto dead;
            }
            Clear_playnote();
            //步进一拍
            ++v;
            if(v == velocity){
                v = 0;
            }
        }
        //乐谱读取结束，把临时潜影盒列剩余的物品全部按规则处理，由于演奏已结束，故删去最后一格剪刀及后续方块即可，无需其他操作
        for(int v = 0; v != velocity; ++v){
            for(int i = 0; i != 25; ++i){
                int j = 26;
                for( ; j >= 0; --j){//从后往前搜索整个箱子，找到最后一个装有剪刀的格子
                    if(temp_shulkerbox_column[v][i][j] == -1){
                        break;
                    }
                }
                if(j < 0){//没有找到剪刀，说明该音符已演奏完毕，剩余的物品不需要装箱
                    continue;
                }else{
                    temp_shulkerbox_column[v][i][j] = 0;//删除这格剪刀，其作用由潜影盒本身替代
                }
                for(int k = j + 1; k < 27; ++k){
                    temp_shulkerbox_column[v][i][k] = 0;
                }
                Package_tsc(v, i);
                if(isDeadbox){//发现死盒，则清零有关乐谱的所有变量，更新起始延迟并重新读取文件，由于这里乐谱读取已结束，故还需重置endWhile
                    count_line = 0;
                    v = 0;
                    endWhile = 0;
                    Clearall();
                    if(isFileInput){
                        fin.close();
                        fin.open(input_file_name.c_str(), std::ios::in);
                    }
                    goto dead;
                }
            }
        }
dead:db = delay_begin;
    }while(isDeadbox);
    if(isFileInput){
        fin.close();
    }
    //进入Package环节
    std::cout << "正在装箱..." << std::endl;
    Package();
    //打印装箱内容到output_file_name_velocity_display.txt中
    std::cout << "打印装箱内容到" << output_file_name << "_velocity" << velocity << "_display" << VERSION_SUFFIX << ".txt中...\n"
              << "注意：-1代表不可堆叠的物品，0~64代表可堆叠的物品数量。\n" << std::endl;
    fout.open((output_file_name + "_velocity" + std::to_string(velocity) + "_display" + VERSION_SUFFIX + ".txt").c_str(), std::ios::out);
    if(!fout.is_open()){
        std::cout << "\n错误：无法打开文件，程序自动关闭。" << std::endl;
        fout.close();
        return 0;
    }
    fout << "注意：-1代表不可堆叠的物品，0~64代表可堆叠的物品数量。\n"
         << "该文件对应曲名及倍速：" << output_file_name << "    " << velocity << "倍速\n"
         << "起始延迟行数：" << delay_begin << "\n"
         << "程序设定的可堆叠物品名称：" << std::endl;
    for(int i = 0; i != 14; ++i){
        fout << blank << STACKABLE_ITEM_NAME[i] << std::endl;
    }
    fout << "程序设定的不可堆叠物品名称：\n" << blank << NON_STACKABLE_ITEM_NAME << std::endl;
    for(int i = 0; i != velocity; ++i){
        fout << "--------------------\n" << std::endl;
        fout << "第" << (i + 1) << "节拍演奏机内容：\n" << std::endl;
        for(int j = 0; j != 25; ++j){
            fout << "    序号" << j << "（" << PITCH[j] << "）" << "单片：\n" << std::endl;
            int temp1 = 0, temp2 = 0;
            fout << "        容器" << temp1 << (isFunnel ? "（漏斗）" : "（大型箱子）") << "：\n" << std::endl;
            for(int k = 0; k != count_shulkerbox_num[i][j]; ++k){
                if((!isFunnel && temp2 == 54) || (isFunnel && temp2 == 5)){
                    temp2 = 0;
                    ++temp1;
                    isFunnel = (!isFunnel);
                    fout << "        容器" << temp1 << (isFunnel ? "（漏斗）" : "（大型箱子）") << "：\n" << std::endl;
                }
                fout << "            第" << (temp2 + 1) << "格潜影盒内容：" << std::endl;
                correct_shulkerbox_array[i][j][k].show(blank);
                ++temp2;
            }
        }
    }
    isFunnel = 0;
    fout.close();
    //填写函数命令
    for(int i = 0; i != velocity; ++i){
        //输出文件命名格式：output_file_name_(当前节拍)in(用户自定义倍速).mcfunction
        std::cout << "正在将命令写入文件" << output_file_name << "_" << (i + 1) << "in" << velocity << VERSION_SUFFIX << ".mcfunction中...";
        if(mode){
            fout.open((output_file_name + "_" + std::to_string(i + 1) + "in" + std::to_string(velocity) + VERSION_SUFFIX + ".mcfunction").c_str(), std::ios::out);
        }else{
            fout.open((output_file_name + "_" + std::to_string(i + 1) + "in" + std::to_string(velocity) + VERSION_SUFFIX + ".mcfunction").c_str(), std::ios::app);
        }
        if(!fout.is_open()){
            std::cout << "\n错误：无法打开文件，程序自动关闭。" << std::endl;
            fout.close();
            return 0;
        }
        for(int j = 0; j != 25; ++j){
            int temp1 = 0, temp2 = 0;
            for(int k = 0; k != count_shulkerbox_num[i][j]; ++k){
                if((!isFunnel && temp2 == 54) || (isFunnel && temp2 == 5)){
                    temp2 = 0;
                    ++temp1;
                    isFunnel = (!isFunnel);
                }
                correct_shulkerbox_array[i][j][k].write_function(j, temp1, temp2, isFunnel);
                ++temp2;
            }
        }
        std::cout << "完成。" << std::endl;
        fout.close();
    }
    fout.close();
    std::cout << "文件写入完成，按任意键关闭程序。" << std::endl;
    system("pause");
    return 0;
}