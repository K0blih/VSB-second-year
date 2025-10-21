using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv6
{
    internal class HourlySalaryEmployee : Employee
    {
        public double HourlySalary { get; set; }
        public int HoursWorked { get; set; }

        public HourlySalaryEmployee(string name, int age, double hourlySalary) : base(name, age)
        {
            HourlySalary = hourlySalary;
        }

        public override double GetSalary()
        {
            return (HoursWorked * HourlySalary + GetBonus());
        }
    }
}
