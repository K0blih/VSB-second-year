using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv6
{
    internal class Worker : HourlySalaryEmployee
    {
        public Worker(string name, int age) : base(name, age, 130) { }

        public override double GetBonus()
        {
            return ((HoursWorked > 160 ? 5000 : 0) + base.GetBonus());
        }
    }
}
