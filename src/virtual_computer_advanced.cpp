#include "../inc/virtual_computer_advanced.h"

#include <iostream>
#include <vector>
static int MEM_VC[500] = {};
static int pc = 0;
std::vector<Cmd*> computer;


void Cmd::execute()
{
    std::cout << "HIHIHI" << std::endl;// TODO-DELETE
    if (isControlOperator(op))
    {
        if (op == 20)
        {
            if (MEM_VC[x])
            {
                pc = z;
            }
            else
            {
                pc++;
            }
        }
        if (op == 30)
        {
            pc = z;
        }
        if (op == 40)
        {
            pc = MEM_VC[z];
        }
    }
    else
    {
        pc++;
        if (op == 0)
        {
            MEM_VC[z] = x;
        }
        if (op == 1)
        {
            MEM_VC[y + MEM_VC[z]] = MEM_VC[x];
        }
        if (op == 2)
        {
            MEM_VC[z] = MEM_VC[x + MEM_VC[y]];
        }
        if (op == 3)
        {
            
            MEM_VC[z] = MEM_VC[x];
        }
        if (4 <= op && op <= 13)
        {
            int ans;
            int _x = MEM_VC[x];
            int _y = MEM_VC[y];
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

            MEM_VC[z] = ans;
        }
        if (op == 14)
        {
            bool xis1 = MEM_VC[x];
            MEM_VC[z] = !xis1;
        }
        if (op == 50)
        {
            std::cout << MEM_VC[x] << " ";
        }
        if (op == 60)
        {
            std::cin >> MEM_VC[x];
        }
    }
}

void Virtual_Computer_Advanced_Main()
{
std::cout << "Hihi" << std::endl;
    int op, x, y, z;
    while (std::cin >> op)
    {
        
        if (op == -1) { break; } // cin negative op to stop the loop;
        // identify the type of this op
        if (isCommand(op))
        {
            if (isBinaryOperator(op)) { std::cin >> x >> y >> z; }
            else if (isUnaryOperatorX(op)) { std::cin >> x; y = z = -1; }
            else if (isUnaryOperatorZ(op)) { std::cin >> z; y = x = -1; }
            else { std::cin >> x >> z; y = -1; }
            Cmd* p = new Cmd(op, x, y, z);
            computer.push_back(p);
        }
    }
    std::cout << "HIHIHI" << std::endl;
    pc = 0;
    while (pc >= 0 && pc < computer.size())
    {
        computer[pc]->execute();
    }
}
