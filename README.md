# custom_redstone_music_player_funnel  
基于漏斗识别的高压红石音乐演奏机  
b站对应视频合集：https://space.bilibili.com/256931342/channel/collectiondetail?sid=3369651&ctype=0  

## 文件说明：  
结构方块文件（.nbt）：请至 https://zh.minecraft.wiki/w/%E7%BB%93%E6%9E%84%E6%96%B9%E5%9D%97 查看结构方块及其文件的使用方式。  

函数文件（.mcfunction）：Java 版函数的用法参见 https://zh.minecraft.wiki/w/Java%E7%89%88%E5%87%BD%E6%95%B0 ，基岩版请根据 https://zh.minecraft.wiki/w/%E5%9F%BA%E5%B2%A9%E7%89%88%E5%87%BD%E6%95%B0 自行调整文件的根目录等。  

数据包：请至 https://zh.minecraft.wiki/w/%E6%95%B0%E6%8D%AE%E5%8C%85 查询自己版本的数据包版本编号，并修改文件中所有 pack.mcmeta 中的 pack_format 一项的值。  

投影文件（.litematic）：请自行查阅相关资料，这里推荐XeKr的视频 https://www.bilibili.com/video/BV1DJ411X78m 。  

C++文件：旧版演奏机的代码建议用Dev-Cpp打开，新版演奏机的代码建议用Vscode打开，否则中文注释可能会乱码。  

可执行文件：不建议直接双击运行新版演奏机的exe文件，中文可能会乱码，建议在Vscode里的终端运行。  

---

### 基于潜影盒的演奏机：  

#### crmpf_workspace  
包含对应演奏机的命令文件、代码、乐谱文件等，使用时需要把命令文件拷贝到数据包里。  
crmpf_shulkerbox_v0.0.cpp：对应第零代基于潜影盒（同时性）的演奏机的代码  
crmpf_shulkerbox_v1.0.cpp：对应第一代基于潜影盒（独立性）的演奏机的代码
exe文件：对应代码的可执行文件，不建议运行  
不带版本号后缀的txt文件：乐谱文件，程序从这些文件读入乐谱  
_display文件：显示乐谱相关信息及演奏机存储系统的具体内容，注意不同代码产生的文件有不同的版本号后缀  
mcfunction文件：对应乐谱的函数文件，带有xiny后缀的是用于倍速播放的不同节拍的文件  

#### generated  
包含演奏机的nbt文件，可借助结构方块放入游戏中。  
crmpf_arc_shulkerbox_w0.0.nbt：第零代基于潜影盒（同时性）的演奏机  
crmpf_arc_shulkerbox_w1.0.nbt：第一代基于潜影盒（独立性）的演奏机  

#### schematics  
包含演奏机的投影文件。  
crmpf_arc_shulkerbox_w0.0.litematic：第零代基于潜影盒（同时性）的演奏机  
crmpf_arc_shulkerbox_w1.0.litematic：第一代基于潜影盒（独立性）的演奏机  
crmpf_arc_shulkerbox_w1.0_quadro.litematic：w1.0版演奏机的适用于倍速播放的四联机版本  

#### nbs2sheets.py  
将nbs文件转换为乐谱文件的python代码。  

---

### 旧版演奏机：  

#### datapacks  
包含对应演奏机的命令文件、代码、乐谱文件等，使用时需要把命令文件拷贝到数据包里。  
##### func  
clearbox.mcfunction: 清空脚下的一个箱子  
clearbox_all_up.mcfunction: 清空演奏机上层的箱子，仅清空一层，不适用于波浪形后端的演奏机  
clearbox_all_down.mcfunction: 清空演奏机下层的箱子，不适用于波浪形后端的演奏机  
clearhopper.mcfunction: 清空脚下两格处的漏斗  
##### music  
C++文件：对应不同版本演奏机的代码，最新的版本是cmmsp_fast.cpp  
txt文件：乐谱文件，程序从这些文件读入乐谱  
mcfunction文件：对应乐谱的函数文件，带有vx_y后缀的是用于倍速播放的不同节拍的文件  
exe文件：对应代码的可执行文件，可直接点击运行  

#### generated  
包含演奏机的nbt文件，可借助结构方块放入游戏中。  
custom_redstone_music_player_funnel.nbt：第一代演奏机（方形前端）  
custom_redstone_music_player_funnel_arc.nbt：第二代演奏机（弧形前端）  
custom_redstone_music_player_funnel_arc_2.nbt：第三代演奏机  
custom_redstone_music_player_funnel_arc_reducible.nbt：第四代演奏机（第一代自动复位演奏机）  
custom_redstone_music_player_funnel_arc_3.nbt：第四代演奏机的无自动复位装置版本  
custom_redstone_music_player_funnel_arc_w0.nbt：第零代模块化演奏机（命令方块销毁）  
custom_redstone_music_player_funnel_arc_w1.nbt：第一代模块化演奏机（自检测销毁）  
crmpfa_reducible_2.nbt：第二代自动复位演奏机（水电梯版）  

#### schematics  
包含演奏机的投影文件。  
目前暂时没有文件。  

---

### 能演奏的钢琴：  
piano.nbt：单音符钢琴（单排15音）  
playable_piano_singlenote.nbt：单音符钢琴（双排25音）  
playable_piano_jukebox.nbt：基于唱片机的简单压缩乐谱钢琴  
playable_piano_halfsize_player.nbt：基于半长度演奏机的钢琴  
custom_redstone_music_player_funnel_arc_w1_piano.nbt：以钢琴为前端的第一代模块化演奏机  
cmmsp_halfsize_piano.cpp：对应半长度演奏机钢琴的代码，请至演奏机文件夹里下载乐谱文件  

---

## 注意：  
1.由于旧版演奏机的代码没有给输出文件加版本号后缀，建议先用程序重新生成一遍函数文件，再拷贝运行。    
