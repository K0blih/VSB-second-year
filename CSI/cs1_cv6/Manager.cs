using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv6
{
    internal class Manager : MonthlySalaryEmployee
    {
        public Manager(string name, int age, double monthlysalary = 80_000) : base(name, age, monthlysalary) { }

        public override double GetSalary()
        {
            return (base.GetSalary() + Age * 500);
        }

        public override void PrintInfo()
        {
            Console.WriteLine("Pozice: manager");
            base.PrintInfo();
        }
    }
}
