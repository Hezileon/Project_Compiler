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




/*0��    �����洢���������Ӧָ������
    0    i        addr
    �������ǽ�����ֵi��ŵ���ַaddr��ָ���ڴ��С�
    ����
    0    100        10
    �ǽ�����100��ŵ���10���洢�ռ��С�
1��    ����洢���������Ӧָ����ʽ�磺
    1    x    B    idx
    ������ַx�д�ŵ�ֵ����ŵ�����B������idx�д�ŵ�ֵ����B + MEM[idx]֮��ĵ�ַ�ռ䡣
2:    ����Ԫ�ض�ȡ
    2    B    idx    x
    ������B������idx�е�ֵ�õ��ĵ�ַ����ŵ�ֵ����ֵ��x��ָ���ַ�У���MEM[B+MEM[idx]]������ַx��
3��    ��ֵ��������
    3    x        y
    ��MEM[x]����y��ָ��ַ
��ͨ��Ŀ�������
    op    x    y    z
    ����MEM[x] op MEM[y]��ֵ��ŵ��ڴ��ַz����
op��ȡֵ�ͺ���Ϊ��
    4    �ӷ�    +
    5    ����    -
    6    �˷�    *
    7    ����    /
    8    ģ����    %
    9    �����ж�    ==
    10    �����ж�    >
    11    С���ж�    <
    12    ������    &&
    13    ������    ||
��Ŀ����ָ�
    14    ������    !

���У����ڡ����ڡ�С���ж�ʱ�������������ô������Ϊ1������Ϊ0
    �롢�򡢷�����Ľ��Ҳ��0��FALSE������1��TRUE��*/