import pynbs # 使用前先在终端输入pip install pynbs
sheet = pynbs.read('C:\\Users\\BCN4M3_18instruments.nbs') # 此处填写用户.nbs文件的绝对路径，注意改用双反斜线，下同
# file=open('C:\\Users\\November in North\\Desktop\\BCN4M3.txt','w')
# print(sheet.header)
for i in range(0, 18): # 18为演奏层数量，.nbs里有音色方块的有几行就改为几
    last_tick = 0
    with open('D:\\Minecraft\\Minecraft 1.17\\bcn4m3_'+str(i)+'.txt', 'w') as file: # 此处填写输出的乐谱文件（.txt格式）的绝对路径，后面的数字代表该乐谱对应原.nbs的第几个演奏层
        for note in sheet.notes:
            if(note.layer == i):
                # print(note.layer,note.tick,note.key-33,file=file)
                tick = note.tick # 获取该音符的时值
                key = note.key - 33 # 该音符的音高 - 33即为0~24
                for j in range(last_tick + 1, tick): # 两个音符时值的差即为休止符的长度
                    print("-1", file = file)
                last_tick = tick
                print(key, file = file)
        print("-2", file = file) # 乐谱结尾
# for tick, chord in pynbs.read('C:\\Users\\November in North\\Desktop\\BCN4M3_18instruments.nbs'):
#   print(tick, [note.key for note in chord])