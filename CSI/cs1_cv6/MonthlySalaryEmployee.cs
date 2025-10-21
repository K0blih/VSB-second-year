using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv6
{
    internal abstract class MonthlySalaryEmployee : Employee
    {
        public double MonthlySalary { get; set; }

        public MonthlySalaryEmployee(string name, int age, double monthlysalary) : base(name, age)
        {
            MonthlySalary = monthlysalary;
        }

        public override double GetSalary()
        {
            return (MonthlySalary + GetBonus());
        }
    }
}
