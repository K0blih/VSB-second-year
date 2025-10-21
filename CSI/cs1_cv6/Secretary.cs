using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv6
{
    internal class Secretary : MonthlySalaryEmployee
    {
        public Secretary(string name, int age, double monthlySalary = 20_000) : base(name, age, monthlySalary) { }

        public override void PrintInfo()
        {
            Console.WriteLine("Pozice: sekretářka");
            base.PrintInfo();
        }
    }
}
