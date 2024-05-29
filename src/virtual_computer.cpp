#include "../inc/virtual_computer.h"

#include <iostream>
#include <map>

static int MEM[500] = {};


static std::map<int, Command*> cid_map; // POSSIBLE IMPROVEMENT seems that can be made global, and save the resource of copying;
// (operated with cid_map) to get the cid's corresponding Complex_Command's ptr;
Command* get_C_Command(int id)
{
    if (cid_map.count(id))
    {
	    return cid_map[id];
    }
    else
    {
        return nullptr;
    }
}
void Create_global_Complex_Command(Command* ptr, int cid)
{
    cid_map[cid] = ptr;
}

void Virtual_Computer_Main()
{
    int N;
    std::cin >> N;
    Command* main_ptr = nullptr;
    int op, x, y, z;
    for (int i = 0; i < N; i++)
    {
        std::cin >> op;
        if (is_single_command(op))
        {
            if (is_binary_operator(op)) { std::cin >> x >> y >> z; }
            else if (is_receiving_single_token(op)) { std::cin >> x; y = z = -1; }
            else { std::cin >> x >> z; y = -1; }
            Command* p = new Single_Command(op, x, y, z);
        }
        else if (is_complex_command(op))
        {
            if (is_binary_operator(op)) { std::cin >> x >> y >> z; }
            else if (is_receiving_single_token(op)) { std::cin >> x; y = z = -1; }
            else { std::cin >> x >> z; y = -1; }
            Command* p = new Complex_Command(op, x, y, z);
            Create_global_Complex_Command(p,z);
            if (z == 1000) { main_ptr = p; }
        }
    }
    main_ptr->execute();
}

void Single_Command::execute()
{
	if(op == 0)
	{
		MEM[z] = x;
	}
	if(op == 1)
	{
        MEM[y + MEM[z]] = MEM[x];
	}
    if(op == 2)
    {
        MEM[z] = MEM[x + MEM[y]];
    }
    if(op == 3)
    {
        MEM[z] = MEM[x];
    }
    if(4 <= op && op <= 13)
    {
        int ans;
        int _x = MEM[x];
        int _y = MEM[y];
        if (op == 4) { ans = _x + _y; }
        if (op == 5) { ans = _x - _y; }
        if (op == 6) { ans = _x * _y; }
        if (op == 7) { ans = _x / _y; }
        if (op == 8) { ans = _x % _y; }
        if (op == 9) { ans = _x == _y; }
        if (op == 10) { ans = _x > _y; }
        if (op == 11) { ans = _x < _y; }
        bool xis1 = _x;
        bool yis1 = _y;
        if (op == 12) { ans = xis1 && yis1; }
        if (op == 13) { ans = xis1 || yis1; }

        MEM[z] = ans;
    }
    if(op == 14)
    {
        bool xis1 = MEM[x];
        MEM[z] = !xis1;
    }
    if(op == 50)
    {
        std::cout << MEM[x] << " ";
    }
    if (op == 60)
    {
        std::cin >> MEM[x];
    }
}

Complex_Command::Complex_Command(int _op, int _N, int _x, int _cid)
    :op(_op), N(_N), x(_x), cid(_cid)
{
    int op, x, y, z;
    for(int i = 0 ; i < N ; i++)
    {
        std::cin >> op;
        if(is_single_command(op))
        {
            if (is_binary_operator(op)){std::cin >> x >> y >> z;}
            else if (is_receiving_single_token(op)) { std::cin >> x; y = z = -1; }
            else{std::cin >> x >> z;y = -1;}
        	Command* p = new Single_Command(op, x, y, z);
        	CCOMMANDS.push_back(p);
        }
        else if(is_complex_command(op))
        {
            if (is_binary_operator(op)) { std::cin >> x >> y >> z; }
            else if (is_receiving_single_token(op)) { std::cin >> x; y = z = -1; }
            else { std::cin >> x >> z; y = -1; }
            Command* p = new Complex_Command(op, x, y, z);
            CCOMMANDS.push_back(p);
        }
        else
        {
            CCOMMANDS.push_back(get_C_Command(op));
        }
    }

}

Complex_Command::Complex_Command(const Complex_Command& cCmd)
	: op(cCmd.op),N(cCmd.N),x(cCmd.x), cid(cCmd.cid)
{
    for (Command* ptr : cCmd.CCOMMANDS)
    {
        opxyz info = ptr->get_info();
        Command* p = new Single_Command(ptr->getOp(), info.x, info.y, info.z);
        CCOMMANDS.push_back(p);
    }
}

void Complex_Command::execute()
{
    if( op == 40)
    {
	    do
	    {
		    for(Command* ptr: CCOMMANDS)
		    {
                ptr->execute();
		    }
        } while (MEM[x] != 0);
    }
    else
    {
        int _x = 0;
        if (op == 20) { _x = 1; }
        else if (op == 30) { _x = MEM[x]; }
        if(_x)
        {
            for (Command* ptr : CCOMMANDS)
            {
                ptr->execute();
            }
        }
    }
}




/*0：    常量存储运算符，相应指令形如
    0    i        addr
    其作用是将常量值i存放到地址addr所指的内存中。
    例如
    0    100        10
    是将常量100存放到第10个存储空间中。
1：    数组存储运算符，相应指令形式如：
    1    x    B    idx
    即将地址x中存放的值，存放到常量B，加上idx中存放的值，即B + MEM[idx]之后的地址空间。
2:    数组元素读取
    2    B    idx    x
    将常量B，加上idx中的值得到的地址所存放的值，赋值到x所指向地址中，即MEM[B+MEM[idx]]赋给地址x。
3：    数值传递运算
    3    x        y
    将MEM[x]赋予y所指地址
普通二目运算符：
    op    x    y    z
    即将MEM[x] op MEM[y]的值存放到内存地址z处。
op的取值和含义为：
    4    加法    +
    5    减法    -
    6    乘法    *
    7    除法    /
    8    模运算    %
    9    等于判断    ==
    10    大于判断    >
    11    小于判断    <
    12    与运算    &&
    13    或运算    ||
单目运算指令：
    14    非运算    !

其中，等于、大于、小于判断时，如果成立，那么运算结果为1；否则为0
    与、或、非运算的结果也是0（FALSE）或者1（TRUE）*/