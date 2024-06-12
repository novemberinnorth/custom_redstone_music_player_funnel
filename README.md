# custom_redstone_music_player_funnel  
基于漏斗识别的高压红石音乐演奏机  
** 文件说明：  
结构方块文件（.nbt）：请至 https://zh.minecraft.wiki/w/%E7%BB%93%E6%9E%84%E6%96%B9%E5%9D%97 查看结构方块及其文件的使用方式。  
custom_redstone_music_player_funnel.nbt：第一代演奏机（方形前端）  
custom_redstone_music_player_funnel_arc.nbt：第二代演奏机（弧形前端）  
custom_redstone_music_player_funnel_arc_2.nbt：第三代演奏机  
custom_redstone_music_player_funnel_arc_reducible.nbt：第四代演奏机  
custom_redstone_music_player_funnel_arc_3.nbt：第四代演奏机的无自动复位装置版本  
带quadro后缀的是相应演奏机的四联机版本  
datapacks.zip：包含所有函数文件（.mcfunction），自定义乐谱程序及其源代码。请至 https://zh.minecraft.wiki/w/%E6%95%B0%E6%8D%AE%E5%8C%85 查询自己版本的数据包版本编号，并修改文件中所有 pack.mcmeta 中的 pack_format 一项的值。  
** 注意：  
customize_mc_music_sheet_plus.cpp及其对应程序仅适用于第四代演奏机及其无自动复位装置版本（之后的版本均采用此程序），sonata_in_a_flat_major_fourth_movement_right.mcfunction作为测试文件，也仅在上述两台装置运行。如想使之在前几代装置上运行，请自行运行 customize_mc_music_sheet.cpp及其对应程序生成新的.mcfunction文件。  
Java 版函数的用法参见 https://zh.minecraft.wiki/w/Java%E7%89%88%E5%87%BD%E6%95%B0 ，基岩版请根据 https://zh.minecraft.wiki/w/%E5%9F%BA%E5%B2%A9%E7%89%88%E5%87%BD%E6%95%B0 自行调整文件的根目录等。  
